#include <Foundation/FoundationPCH.h>

#include <Foundation/IO/CompressedStreamZlib.h>
#include <Foundation/Math/Math.h>

#ifdef BUILDSYSTEM_ENABLE_ZLIB_SUPPORT

#  include <zlib/zlib.h>

static voidpf zLibAlloc OF((voidpf opaque, uInt items, uInt size))
{
  PL_IGNORE_UNUSED(opaque);
  return PL_DEFAULT_NEW_RAW_BUFFER(plUInt8, plMath::SafeConvertToSizeT(plMath::SafeMultiply64(items, size)));
}

static void zLibFree OF((voidpf opaque, voidpf address))
{
  PL_IGNORE_UNUSED(opaque);
  plUInt8* pData = (plUInt8*)address;
  PL_DEFAULT_DELETE_RAW_BUFFER(pData);
}

PL_DEFINE_AS_POD_TYPE(z_stream_s);

plCompressedStreamReaderZip::plCompressedStreamReaderZip() = default;

plCompressedStreamReaderZip::~plCompressedStreamReaderZip()
{
  PL_VERIFY(inflateEnd(m_pZLibStream) == Z_OK, "Deinitializing the zlib stream failed: '{0}'", m_pZLibStream->msg);
  PL_DEFAULT_DELETE(m_pZLibStream);
}

void plCompressedStreamReaderZip::SetInputStream(plStreamReader* pInputStream, plUInt64 uiInputSize)
{
  if (m_pZLibStream)
  {
    PL_VERIFY(inflateEnd(m_pZLibStream) == Z_OK, "Deinitializing the zlib stream failed: '{0}'", m_pZLibStream->msg);
    PL_DEFAULT_DELETE(m_pZLibStream);
  }

  m_CompressedCache.SetCountUninitialized(1024 * 4);
  m_bReachedEnd = false;
  m_pInputStream = pInputStream;
  m_uiRemainingInputSize = uiInputSize;

  {
    m_pZLibStream = PL_DEFAULT_NEW(z_stream_s);
    plMemoryUtils::ZeroFill(m_pZLibStream, 1);

    m_pZLibStream->opaque = nullptr;
    m_pZLibStream->zalloc = zLibAlloc;
    m_pZLibStream->zfree = zLibFree;

    PL_VERIFY(inflateInit2(m_pZLibStream, -MAX_WBITS) == Z_OK, "Initializing the zip stream for decompression failed: '{0}'", m_pZLibStream->msg);
  }
}

plUInt64 plCompressedStreamReaderZip::ReadBytes(void* pReadBuffer, plUInt64 uiBytesToRead)
{
  if (uiBytesToRead == 0 || m_bReachedEnd)
    return 0;

  // Implement the 'skip n bytes' feature with a temp cache
  if (pReadBuffer == nullptr)
  {
    plUInt64 uiBytesRead = 0;
    plUInt8 uiTemp[1024];

    while (uiBytesToRead > 0)
    {
      const plUInt32 uiToRead = plMath::Min<plUInt32>(static_cast<plUInt32>(uiBytesToRead), 1024);

      const plUInt64 uiGotBytes = ReadBytes(uiTemp, uiToRead);

      uiBytesRead += uiGotBytes;
      uiBytesToRead -= uiGotBytes;

      if (uiGotBytes == 0) // prevent an endless loop
        break;
    }

    return uiBytesRead;
  }


  m_pZLibStream->next_out = static_cast<Bytef*>(pReadBuffer);
  m_pZLibStream->avail_out = static_cast<plUInt32>(uiBytesToRead);
  m_pZLibStream->total_out = 0;

  while (m_pZLibStream->avail_out > 0)
  {
    // if our input buffer is empty, we need to read more into our cache
    if (m_pZLibStream->avail_in == 0 && m_uiRemainingInputSize > 0)
    {
      plUInt64 uiReadAmount = m_CompressedCache.GetCount();
      if (m_uiRemainingInputSize < uiReadAmount)
      {
        uiReadAmount = m_uiRemainingInputSize;
      }
      if (uiReadAmount == 0)
      {
        m_bReachedEnd = true;
        return m_pZLibStream->total_out;
      }

      PL_VERIFY(m_pInputStream->ReadBytes(m_CompressedCache.GetData(), sizeof(plUInt8) * uiReadAmount) == sizeof(plUInt8) * uiReadAmount, "Reading the compressed chunk of size {0} from the input stream failed.", uiReadAmount);
      m_pZLibStream->avail_in = static_cast<uInt>(uiReadAmount);
      m_pZLibStream->next_in = m_CompressedCache.GetData();
      m_uiRemainingInputSize -= uiReadAmount;
    }

    const int iRet = inflate(m_pZLibStream, Z_SYNC_FLUSH);
    PL_ASSERT_DEV(iRet == Z_OK || iRet == Z_STREAM_END, "Decompressing the stream failed: '{0}'", m_pZLibStream->msg);

    if (iRet == Z_STREAM_END)
    {
      m_bReachedEnd = true;
      PL_ASSERT_DEV(m_pZLibStream->avail_in == 0, "The input buffer should be depleted, but {0} bytes are still there.", m_pZLibStream->avail_in);
      return m_pZLibStream->total_out;
    }
  }

  return m_pZLibStream->total_out;
}


//////////////////////////////////////////////////////////////////////////


plCompressedStreamReaderZlib::plCompressedStreamReaderZlib(plStreamReader* pInputStream)
  : m_pInputStream(pInputStream)
{
  m_CompressedCache.SetCountUninitialized(1024 * 4);
}

plCompressedStreamReaderZlib::~plCompressedStreamReaderZlib()
{
  PL_VERIFY(inflateEnd(m_pZLibStream) == Z_OK, "Deinitializing the zlib stream failed: '{0}'", m_pZLibStream->msg);

  PL_DEFAULT_DELETE(m_pZLibStream);
}

plUInt64 plCompressedStreamReaderZlib::ReadBytes(void* pReadBuffer, plUInt64 uiBytesToRead)
{
  if (uiBytesToRead == 0 || m_bReachedEnd)
    return 0;

  // if we have not read from the stream before, initialize everything
  if (m_pZLibStream == nullptr)
  {
    m_pZLibStream = PL_DEFAULT_NEW(z_stream_s);
    plMemoryUtils::ZeroFill(m_pZLibStream, 1);

    m_pZLibStream->opaque = nullptr;
    m_pZLibStream->zalloc = zLibAlloc;
    m_pZLibStream->zfree = zLibFree;

    PL_VERIFY(inflateInit(m_pZLibStream) == Z_OK, "Initializing the zlib stream for decompression failed: '{0}'", m_pZLibStream->msg);
  }

  // Implement the 'skip n bytes' feature with a temp cache
  if (pReadBuffer == nullptr)
  {
    plUInt64 uiBytesRead = 0;
    plUInt8 uiTemp[1024];

    while (uiBytesToRead > 0)
    {
      const plUInt32 uiToRead = plMath::Min<plUInt32>(static_cast<plUInt32>(uiBytesToRead), 1024);

      const plUInt64 uiGotBytes = ReadBytes(uiTemp, uiToRead);

      uiBytesRead += uiGotBytes;
      uiBytesToRead -= uiGotBytes;

      if (uiGotBytes == 0) // prevent an endless loop
        break;
    }

    return uiBytesRead;
  }


  m_pZLibStream->next_out = static_cast<Bytef*>(pReadBuffer);
  m_pZLibStream->avail_out = static_cast<plUInt32>(uiBytesToRead);
  m_pZLibStream->total_out = 0;

  while (m_pZLibStream->avail_out > 0)
  {
    // if our input buffer is empty, we need to read more into our cache
    if (m_pZLibStream->avail_in == 0)
    {
      plUInt16 uiCompressedSize = 0;
      PL_VERIFY(m_pInputStream->ReadBytes(&uiCompressedSize, sizeof(plUInt16)) == sizeof(plUInt16), "Reading the compressed chunk size from the input stream failed.");

      m_pZLibStream->avail_in = uiCompressedSize;
      m_pZLibStream->next_in = m_CompressedCache.GetData();

      if (uiCompressedSize > 0)
      {
        PL_VERIFY(m_pInputStream->ReadBytes(m_CompressedCache.GetData(), sizeof(plUInt8) * uiCompressedSize) == sizeof(plUInt8) * uiCompressedSize, "Reading the compressed chunk of size {0} from the input stream failed.", uiCompressedSize);
      }
    }

    // if the input buffer is still empty, there was no more data to read (we reached the zero-terminator)
    if (m_pZLibStream->avail_in == 0)
    {
      // in this case there is also no output that can be generated anymore
      m_bReachedEnd = true;
      return m_pZLibStream->total_out;
    }

    const int iRet = inflate(m_pZLibStream, Z_NO_FLUSH);
    PL_ASSERT_DEV(iRet == Z_OK || iRet == Z_STREAM_END, "Decompressing the stream failed: '{0}'", m_pZLibStream->msg);

    if (iRet == Z_STREAM_END)
    {
      m_bReachedEnd = true;

      // if we have reached the end, we have not yet read the zero-terminator
      // do this now, so that data that comes after the compressed stream can be read properly

      plUInt16 uiTerminator = 0;
      PL_VERIFY(m_pInputStream->ReadBytes(&uiTerminator, sizeof(plUInt16)) == sizeof(plUInt16), "Reading the compressed stream terminator failed.");

      PL_ASSERT_DEV(uiTerminator == 0, "Unexpected Stream Terminator: {0}", uiTerminator);
      PL_ASSERT_DEV(m_pZLibStream->avail_in == 0, "The input buffer should be depleted, but {0} bytes are still there.", m_pZLibStream->avail_in);
      return m_pZLibStream->total_out;
    }
  }

  return m_pZLibStream->total_out;
}


plCompressedStreamWriterZlib::plCompressedStreamWriterZlib(plStreamWriter* pOutputStream, Compression ratio)
  : m_pOutputStream(pOutputStream)
{
  m_CompressedCache.SetCountUninitialized(1024 * 4);

  m_pZLibStream = PL_DEFAULT_NEW(z_stream_s);

  plMemoryUtils::ZeroFill(m_pZLibStream, 1);

  m_pZLibStream->opaque = nullptr;
  m_pZLibStream->zalloc = zLibAlloc;
  m_pZLibStream->zfree = zLibFree;
  m_pZLibStream->next_out = m_CompressedCache.GetData();
  m_pZLibStream->avail_out = m_CompressedCache.GetCount();
  m_pZLibStream->total_out = 0;

  PL_VERIFY(deflateInit(m_pZLibStream, ratio) == Z_OK, "Initializing the zlib stream for compression failed: '{0}'", m_pZLibStream->msg);
}

plCompressedStreamWriterZlib::~plCompressedStreamWriterZlib()
{
  CloseStream().IgnoreResult();
}

plResult plCompressedStreamWriterZlib::CloseStream()
{
  if (m_pZLibStream == nullptr)
    return PL_SUCCESS;

  plInt32 iRes = Z_OK;
  while (iRes == Z_OK)
  {
    if (m_pZLibStream->avail_out == 0)
    {
      if (Flush() == PL_FAILURE)
        return PL_FAILURE;
    }

    iRes = deflate(m_pZLibStream, Z_FINISH);
    PL_ASSERT_DEV(iRes == Z_STREAM_END || iRes == Z_OK, "Finishing the stream failed: '{0}'", m_pZLibStream->msg);
  }

  // one more flush to write out the last chunk
  if (Flush() == PL_FAILURE)
    return PL_FAILURE;

  // write a zero-terminator
  const plUInt16 uiTerminator = 0;
  if (m_pOutputStream->WriteBytes(&uiTerminator, sizeof(plUInt16)) == PL_FAILURE)
    return PL_FAILURE;

  PL_VERIFY(deflateEnd(m_pZLibStream) == Z_OK, "Deinitializing the zlib compression stream failed: '{0}'", m_pZLibStream->msg);
  PL_DEFAULT_DELETE(m_pZLibStream);

  return PL_SUCCESS;
}

plResult plCompressedStreamWriterZlib::Flush()
{
  if (m_pZLibStream == nullptr)
    return PL_SUCCESS;

  const plUInt16 uiUsedCache = static_cast<plUInt16>(m_pZLibStream->total_out);

  if (uiUsedCache == 0)
    return PL_SUCCESS;

  if (m_pOutputStream->WriteBytes(&uiUsedCache, sizeof(plUInt16)) == PL_FAILURE)
    return PL_FAILURE;

  if (m_pOutputStream->WriteBytes(m_CompressedCache.GetData(), sizeof(plUInt8) * uiUsedCache) == PL_FAILURE)
    return PL_FAILURE;

  m_uiCompressedSize += uiUsedCache;

  m_pZLibStream->total_out = 0;
  m_pZLibStream->next_out = m_CompressedCache.GetData();
  m_pZLibStream->avail_out = m_CompressedCache.GetCount();

  return PL_SUCCESS;
}

plResult plCompressedStreamWriterZlib::WriteBytes(const void* pWriteBuffer, plUInt64 uiBytesToWrite)
{
  PL_ASSERT_DEV(m_pZLibStream != nullptr, "The stream is already closed, you cannot write more data to it.");

  m_uiUncompressedSize += uiBytesToWrite;

  m_pZLibStream->next_in = static_cast<Bytef*>(const_cast<void*>(pWriteBuffer)); // C libraries suck at type safety
  m_pZLibStream->avail_in = static_cast<plUInt32>(uiBytesToWrite);
  m_pZLibStream->total_in = 0;

  while (m_pZLibStream->avail_in > 0)
  {
    if (m_pZLibStream->avail_out == 0)
    {
      if (Flush() == PL_FAILURE)
        return PL_FAILURE;
    }

    PL_VERIFY(deflate(m_pZLibStream, Z_NO_FLUSH) == Z_OK, "Compressing the zlib stream failed: '{0}'", m_pZLibStream->msg);
  }

  return PL_SUCCESS;
}

#endif // BUILDSYSTEM_ENABLE_ZLIB_SUPPORT
