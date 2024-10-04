#pragma once

#include <Foundation/IO/Archive/ArchiveReader.h>
#include <Foundation/IO/CompressedStreamZlib.h>
#include <Foundation/IO/CompressedStreamZstd.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/Implementation/DataDirType.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Time/Timestamp.h>

class plArchiveEntry;

namespace plDataDirectory
{
  class ArchiveReaderUncompressed;
  class ArchiveReaderZstd;
  class ArchiveReaderZip;

  class PL_FOUNDATION_DLL ArchiveType : public plDataDirectoryType
  {
  public:
    ArchiveType();
    ~ArchiveType();

    static plDataDirectoryType* Factory(plStringView sDataDirectory, plStringView sGroup, plStringView sRootName, plDataDirUsage usage);

    virtual const plString128& GetRedirectedDataDirectoryPath() const override { return m_sRedirectedDataDirPath; }

  protected:
    virtual plDataDirectoryReader* OpenFileToRead(plStringView sFile, plFileShareMode::Enum FileShareMode, bool bSpecificallyThisDataDir) override;

    virtual void RemoveDataDirectory() override;

    virtual bool ExistsFile(plStringView sFile, bool bOneSpecificDataDir) override;

    virtual plResult GetFileStats(plStringView sFileOrFolder, bool bOneSpecificDataDir, plFileStats& out_Stats) override;

    virtual plResult InternalInitializeDataDirectory(plStringView sDirectory) override;

    virtual void OnReaderWriterClose(plDataDirectoryReaderWriterBase* pClosed) override;

    plString128 m_sRedirectedDataDirPath;
    plString32 m_sArchiveSubFolder;
    plTimestamp m_LastModificationTime;
    plArchiveReader m_ArchiveReader;

    plMutex m_ReaderMutex;
    plHybridArray<plUniquePtr<ArchiveReaderUncompressed>, 4> m_ReadersUncompressed;
    plHybridArray<ArchiveReaderUncompressed*, 4> m_FreeReadersUncompressed;

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
    plHybridArray<plUniquePtr<ArchiveReaderZstd>, 4> m_ReadersZstd;
    plHybridArray<ArchiveReaderZstd*, 4> m_FreeReadersZstd;
#endif
#ifdef BUILDSYSTEM_ENABLE_ZLIB_SUPPORT
    plHybridArray<plUniquePtr<ArchiveReaderZip>, 4> m_ReadersZip;
    plHybridArray<ArchiveReaderZip*, 4> m_FreeReadersZip;
#endif
  };

  class PL_FOUNDATION_DLL ArchiveReaderCommon : public plDataDirectoryReader
  {
    PL_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderCommon);

  public:
    ArchiveReaderCommon(plInt32 iDataDirUserData);

    virtual plUInt64 GetFileSize() const override;

  protected:
    friend class ArchiveType;

    plUInt64 m_uiUncompressedSize = 0;
    plUInt64 m_uiCompressedSize = 0;
    plRawMemoryStreamReader m_MemStreamReader;
  };

  class PL_FOUNDATION_DLL ArchiveReaderUncompressed : public ArchiveReaderCommon
  {
    PL_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderUncompressed);

  public:
    ArchiveReaderUncompressed(plInt32 iDataDirUserData);

    virtual plUInt64 Skip(plUInt64 uiBytes) override;
    virtual plUInt64 Read(void* pBuffer, plUInt64 uiBytes) override;

  protected:
    virtual plResult InternalOpen(plFileShareMode::Enum FileShareMode) override;
    virtual void InternalClose() override;
  };

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
  class PL_FOUNDATION_DLL ArchiveReaderZstd : public ArchiveReaderCommon
  {
    PL_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderZstd);

  public:
    ArchiveReaderZstd(plInt32 iDataDirUserData);

    virtual plUInt64 Read(void* pBuffer, plUInt64 uiBytes) override;

  protected:
    virtual plResult InternalOpen(plFileShareMode::Enum FileShareMode) override;
    virtual void InternalClose() override;

    plCompressedStreamReaderZstd m_CompressedStreamReader;
  };
#endif

#ifdef BUILDSYSTEM_ENABLE_ZLIB_SUPPORT
  /// \brief Allows reading of zip / apk containers.
  /// Needed to allow Android to read data from the apk.
  class PL_FOUNDATION_DLL ArchiveReaderZip : public ArchiveReaderUncompressed
  {
    PL_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderZip);

  public:
    ArchiveReaderZip(plInt32 iDataDirUserData);
    ~ArchiveReaderZip();

    virtual plUInt64 Read(void* pBuffer, plUInt64 uiBytes) override;

  protected:
    virtual plResult InternalOpen(plFileShareMode::Enum FileShareMode) override;

    friend class ArchiveType;

    plCompressedStreamReaderZip m_CompressedStreamReader;
  };
#endif
} // namespace plDataDirectory
