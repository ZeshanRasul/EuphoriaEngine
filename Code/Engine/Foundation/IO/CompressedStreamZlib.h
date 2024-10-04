
#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/IO/Stream.h>

#ifdef BUILDSYSTEM_ENABLE_ZLIB_SUPPORT

struct z_stream_s;

/// \brief Part of the zip / apk support of the plArchiveReader.
/// Needed to allow Android to read data from the apk.
class PL_FOUNDATION_DLL plCompressedStreamReaderZip : public plStreamReader
{
public:
  plCompressedStreamReaderZip();
  ~plCompressedStreamReaderZip();

  /// \brief Configures the reader to decompress the data from the given input stream.
  ///
  /// Calling this a second time on the same instance is valid and allows to reuse the decoder, which is more efficient than creating a new
  /// one.
  void SetInputStream(plStreamReader* pInputStream, plUInt64 uiInputSize);

  /// \brief Reads either uiBytesToRead or the amount of remaining bytes in the stream into pReadBuffer.
  ///
  /// It is valid to pass nullptr for pReadBuffer, in this case the memory stream position is only advanced by the given number of bytes.
  /// However, since this is a compressed stream, the decompression still needs to be done, so this won't save any time.
  virtual plUInt64 ReadBytes(void* pReadBuffer, plUInt64 uiBytesToRead) override; // [tested]

private:
  plUInt64 m_uiRemainingInputSize = 0;
  bool m_bReachedEnd = false;
  plDynamicArray<plUInt8> m_CompressedCache;
  plStreamReader* m_pInputStream = nullptr;
  z_stream_s* m_pZLibStream = nullptr;
};


/// \brief A stream reader that will decompress data that was stored using the plCompressedStreamWriterZlib.
///
/// The reader takes another reader as its data source (e.g. a file or a memory stream). The compressed reader
/// uses a cache of 256 Bytes internally to prevent excessive reads from its source and to improve decompression speed.
class PL_FOUNDATION_DLL plCompressedStreamReaderZlib : public plStreamReader
{
public:
  /// \brief Takes an input stream as the source from which to read the compressed data.
  plCompressedStreamReaderZlib(plStreamReader* pInputStream); // [tested]

  ~plCompressedStreamReaderZlib();                            // [tested]

  /// \brief Reads either uiBytesToRead or the amount of remaining bytes in the stream into pReadBuffer.
  ///
  /// It is valid to pass nullptr for pReadBuffer, in this case the memory stream position is only advanced by the given number of bytes.
  /// However, since this is a compressed stream, the decompression still needs to be done, so this won't save any time.
  virtual plUInt64 ReadBytes(void* pReadBuffer, plUInt64 uiBytesToRead) override; // [tested]

private:
  bool m_bReachedEnd = false;
  plDynamicArray<plUInt8> m_CompressedCache;
  plStreamReader* m_pInputStream = nullptr;
  z_stream_s* m_pZLibStream = nullptr;
};

/// \brief A stream writer that will compress all incoming data and then passes it on into another stream.
///
/// The stream uses an internal cache of 255 Bytes to compress data, before it passes that on to the output stream.
/// It does not need to compress the entire data first, and it will not do any dynamic memory allocations.
/// Calling Flush() will write the current amount of compressed data to the output stream. Calling this frequently might reduce the
/// compression ratio and it should only be used to reduce output lag. However, there is absolutely no guarantee that all the data that was
/// put into the stream will be readable from the output stream, after calling Flush(). In fact, it is quite likely that a large amount of
/// data has still not been written to it, because it is still inside the compressor.
class PL_FOUNDATION_DLL plCompressedStreamWriterZlib : public plStreamWriter
{
public:
  /// \brief Specifies the compression level of the stream.
  enum Compression
  {
    Uncompressed = 0,
    Fastest = 1,
    Fast = 3,
    Average = 5,
    High = 7,
    Highest = 9,
    Default = Fastest ///< Should be preferred, good compression and good speed. Higher compression ratios save not much space but take
                      ///< considerably longer.
  };

  /// \brief The constructor takes another stream writer to pass the output into, and a compression level.
  plCompressedStreamWriterZlib(plStreamWriter* pOutputStream, Compression ratio = Compression::Default); // [tested]

  /// \brief Calls CloseStream() internally.
  ~plCompressedStreamWriterZlib(); // [tested]

  /// \brief Compresses \a uiBytesToWrite from \a pWriteBuffer.
  ///
  /// Will output bursts of 256 bytes to the output stream every once in a while.
  virtual plResult WriteBytes(const void* pWriteBuffer, plUInt64 uiBytesToWrite) override; // [tested]

  /// \brief Finishes the stream and writes all remaining data to the output stream.
  ///
  /// After calling this function, no more data can be written to the stream. GetCompressedSize() will return the final compressed size
  /// of the data.
  /// Note that this function is not the same as Flush(), since Flush() assumes that more data can be written to the stream afterwards,
  /// which is not the case for CloseStream().
  plResult CloseStream(); // [tested]

  /// \brief Returns the size of the data in its uncompressed state.
  plUInt64 GetUncompressedSize() const { return m_uiUncompressedSize; } // [tested]

  /// \brief Returns the current compressed size of the data.
  ///
  /// This value is only accurate after CloseStream() has been called. Before that it is only a rough value, because a lot of data
  /// might still be cached and not yet accounted for.
  /// Note that GetCompressedSize() returns the compressed size of the data, not the size of the data that was written to the output stream,
  /// which will be larger (1 additional byte per 255 compressed bytes, plus one zero terminator byte).
  plUInt64 GetCompressedSize() const { return m_uiCompressedSize; } // [tested]

  /// \brief Writes the currently available compressed data to the stream.
  ///
  /// This does NOT guarantee that you can read all the uncompressed data from the output stream afterwards, because a lot of data
  /// will still be inside the compressor and thus not yet written to the stream.
  virtual plResult Flush() override;

private:
  plUInt64 m_uiUncompressedSize = 0;
  plUInt64 m_uiCompressedSize = 0;

  plStreamWriter* m_pOutputStream = nullptr;
  z_stream_s* m_pZLibStream = nullptr;

  plDynamicArray<plUInt8> m_CompressedCache;
};

#endif // BUILDSYSTEM_ENABLE_ZLIB_SUPPORT