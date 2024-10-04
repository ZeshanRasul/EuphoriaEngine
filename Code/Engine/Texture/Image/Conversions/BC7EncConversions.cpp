#include <Texture/TexturePCH.h>

#if PL_USE_BC7ENC

#  include <bc7enc_rdo/rdo_bc_encoder.h>

#  include <Foundation/System/SystemInformation.h>
#  include <Texture/Image/ImageConversion.h>

plImageConversionEntry g_BC7EncConversions[] = {
  // Even at the lowest quality level of BC7Enc, BC1 encoding times are more than a magnitude worse than DXTexConv.
  // plImageConversionEntry(plImageFormat::R8G8B8A8_UNORM, plImageFormat::BC1_UNORM, plImageConversionFlags::Default),
  // plImageConversionEntry(plImageFormat::R8G8B8A8_UNORM_SRGB, plImageFormat::BC1_UNORM_SRGB, plImageConversionFlags::Default),
  plImageConversionEntry(plImageFormat::R8G8B8A8_UNORM, plImageFormat::BC7_UNORM, plImageConversionFlags::Default),
  plImageConversionEntry(plImageFormat::R8G8B8A8_UNORM_SRGB, plImageFormat::BC7_UNORM_SRGB, plImageConversionFlags::Default),
};

class plImageConversion_CompressBC7Enc : public plImageConversionStepCompressBlocks
{
public:
  virtual plArrayPtr<const plImageConversionEntry> GetSupportedConversions() const override
  {
    return g_BC7EncConversions;
  }

  virtual plResult CompressBlocks(plConstByteBlobPtr source, plByteBlobPtr target, plUInt32 numBlocksX, plUInt32 numBlocksY,
    plImageFormat::Enum sourceFormat, plImageFormat::Enum targetFormat) const override
  {
    plSystemInformation info = plSystemInformation::Get();
    const plInt32 iCpuCores = info.GetCPUCoreCount();

    rdo_bc::rdo_bc_params rp;
    rp.m_rdo_max_threads = plMath::Clamp<plInt32>(iCpuCores - 2, 2, 8);
    rp.m_status_output = false;
    rp.m_bc1_quality_level = 18;

    switch (targetFormat)
    {
      case plImageFormat::BC7_UNORM:
      case plImageFormat::BC7_UNORM_SRGB:
        rp.m_dxgi_format = DXGI_FORMAT_BC7_UNORM;
        break;
      case plImageFormat::BC1_UNORM:
      case plImageFormat::BC1_UNORM_SRGB:
        rp.m_dxgi_format = DXGI_FORMAT_BC1_UNORM;
        break;
      default:
        PL_ASSERT_NOT_IMPLEMENTED;
    }

    utils::image_u8 source_image(numBlocksX * 4, numBlocksY * 4);
    auto& pixels = source_image.get_pixels();
    plMemoryUtils::Copy<plUInt32>(reinterpret_cast<plUInt32*>(pixels.data()), reinterpret_cast<const plUInt32*>(source.GetPtr()), numBlocksX * 4 * numBlocksY * 4);

    rdo_bc::rdo_bc_encoder encoder;
    if (!encoder.init(source_image, rp))
    {
      plLog::Error("rdo_bc_encoder::init() failed!");
      return PL_FAILURE;
    }

    if (!encoder.encode())
    {
      plLog::Error("rdo_bc_encoder::encode() failed!");
      return PL_FAILURE;
    }

    const plUInt32 uiTotalBytes = encoder.get_total_blocks_size_in_bytes();
    if (uiTotalBytes != target.GetCount())
    {
      plLog::Error("Encoder output of {} byte does not match the expected size of {} bytes", uiTotalBytes, target.GetCount());
      return PL_FAILURE;
    }
    plMemoryUtils::Copy<plUInt8>(reinterpret_cast<plUInt8*>(target.GetPtr()), reinterpret_cast<const plUInt8*>(encoder.get_blocks()), uiTotalBytes);
    return PL_SUCCESS;
  }
};

// PL_STATICLINK_FORCE
static plImageConversion_CompressBC7Enc s_conversion_compressBC7Enc;

#endif

PL_STATICLINK_FILE(Texture, Texture_Image_Conversions_BC7EncConversions);