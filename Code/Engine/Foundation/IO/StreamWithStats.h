
#pragma once

#include <Foundation/IO/Stream.h>

/// \brief A stream reader that wraps another stream to track how many bytes are read from it.
class PL_FOUNDATION_DLL plStreamReaderWithStats : public plStreamReader
{
public:
  plStreamReaderWithStats() = default;
  plStreamReaderWithStats(plStreamReader* pStream)
    : m_pStream(pStream)
  {
  }

  virtual plUInt64 ReadBytes(void* pReadBuffer, plUInt64 uiBytesToRead) override
  {
    const plUInt64 uiRead = m_pStream->ReadBytes(pReadBuffer, uiBytesToRead);
    m_uiBytesRead += uiRead;
    return uiRead;
  }

  plUInt64 SkipBytes(plUInt64 uiBytesToSkip) override
  {
    const plUInt64 uiSkipped = m_pStream->SkipBytes(uiBytesToSkip);
    m_uiBytesSkipped += uiSkipped;
    return uiSkipped;
  }

  /// the stream to forward all requests to
  plStreamReader* m_pStream = nullptr;

  /// the number of bytes that were read from the wrapped stream
  /// public access so that users can read and modify this in case they want to reset the value at any time
  plUInt64 m_uiBytesRead = 0;

  /// the number of bytes that were skipped from the wrapped stream
  plUInt64 m_uiBytesSkipped = 0;
};

/// \brief A stream writer that wraps another stream to track how many bytes are written to it.
class PL_FOUNDATION_DLL plStreamWriterWithStats : public plStreamWriter
{
public:
  plStreamWriterWithStats() = default;
  plStreamWriterWithStats(plStreamWriter* pStream)
    : m_pStream(pStream)
  {
  }

  virtual plResult WriteBytes(const void* pWriteBuffer, plUInt64 uiBytesToWrite) override
  {
    m_uiBytesWritten += uiBytesToWrite;
    return m_pStream->WriteBytes(pWriteBuffer, uiBytesToWrite);
  }

  plResult Flush() override
  {
    return m_pStream->Flush();
  }

  /// the stream to forward all requests to
  plStreamWriter* m_pStream = nullptr;

  /// the number of bytes that were written to the wrapped stream
  /// public access so that users can read and modify this in case they want to reset the value at any time
  plUInt64 m_uiBytesWritten = 0;
};
