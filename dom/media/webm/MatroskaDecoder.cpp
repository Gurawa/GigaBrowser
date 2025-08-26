/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MatroskaDecoder.h"

#include <utility>

#include "VPXDecoder.h"
#include "mozilla/Preferences.h"
#include "mozilla/StaticPrefs_media.h"
#ifdef MOZ_AV1
#  include "AOMDecoder.h"
#endif
#include "MediaContainerType.h"
#include "PDMFactory.h"
#include "PlatformDecoderModule.h"
#include "VideoUtils.h"

namespace mozilla {

/* static */
nsTArray<UniquePtr<TrackInfo>> MatroskaDecoder::GetTracksInfo(
    const MediaContainerType& aType, MediaResult& aError) {
  nsTArray<UniquePtr<TrackInfo>> tracks;
  const bool isVideo = aType.Type() == MEDIAMIMETYPE("video/x-matroska") || 
                       aType.Type() == MEDIAMIMETYPE("video/mkv");

  if (aType.Type() != MEDIAMIMETYPE("audio/x-matroska") && 
      aType.Type() != MEDIAMIMETYPE("audio/mkv") && !isVideo) {
    aError = MediaResult(
        NS_ERROR_DOM_MEDIA_FATAL_ERR,
        RESULT_DETAIL("Invalid type:%s", aType.Type().AsString().get()));
    return tracks;
  }

  aError = NS_OK;

  const MediaCodecs& codecs = aType.ExtendedType().Codecs();
  if (codecs.IsEmpty()) {
    return tracks;
  }

  for (const auto& codec : codecs.Range()) {
    if (codec.EqualsLiteral("opus") || codec.EqualsLiteral("vorbis") ||
        codec.EqualsLiteral("mp4a") || codec.Find("mp4a."_ns) == 0 ||
        codec.EqualsLiteral("aac") || codec.Find("aac."_ns) == 0) {
      nsString mimeType;
      if (codec.EqualsLiteral("opus")) {
        mimeType = u"audio/opus"_ns;
      } else if (codec.EqualsLiteral("vorbis")) {
        mimeType = u"audio/vorbis"_ns;
      } else {
        mimeType = u"audio/mp4a-latm"_ns;
      }
      tracks.AppendElement(
          CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
              NS_ConvertUTF16toUTF8(mimeType), aType));
      continue;
    }
    if (isVideo) {
      UniquePtr<TrackInfo> trackInfo;
      if (IsVP9CodecString(codec)) {
        trackInfo = CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
            "video/vp9"_ns, aType);
      } else if (IsVP8CodecString(codec)) {
        trackInfo = CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
            "video/vp8"_ns, aType);
      } else if (codec.EqualsLiteral("avc1") || codec.Find("avc1."_ns) == 0 ||
                 codec.EqualsLiteral("h264") || codec.Find("h264."_ns) == 0) {
        trackInfo = CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
            "video/avc"_ns, aType);
      } else if (codec.EqualsLiteral("hvc1") || codec.Find("hvc1."_ns) == 0 ||
                 codec.EqualsLiteral("hev1") || codec.Find("hev1."_ns) == 0 ||
                 codec.EqualsLiteral("hevc") || codec.Find("hevc."_ns) == 0) {
        trackInfo = CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
            "video/hevc"_ns, aType);
      }
      if (trackInfo) {
        if (IsVP9CodecString(codec) || IsVP8CodecString(codec)) {
          VPXDecoder::SetVideoInfo(trackInfo->GetAsVideoInfo(), codec);
        }
        tracks.AppendElement(std::move(trackInfo));
        continue;
      }
    }
#ifdef MOZ_AV1
    if (StaticPrefs::media_av1_enabled() && IsAV1CodecString(codec)) {
      auto trackInfo =
          CreateTrackInfoWithMIMETypeAndContainerTypeExtraParameters(
              "video/av1"_ns, aType);
      AOMDecoder::SetVideoInfo(trackInfo->GetAsVideoInfo(), codec);
      tracks.AppendElement(std::move(trackInfo));
      continue;
    }
#endif
    // Unknown codec
    aError = MediaResult(
        NS_ERROR_DOM_MEDIA_FATAL_ERR,
        RESULT_DETAIL("Unknown codec:%s", NS_ConvertUTF16toUTF8(codec).get()));
  }
  return tracks;
}

/* static */
bool MatroskaDecoder::IsSupportedType(const MediaContainerType& aContainerType) {
  if (!StaticPrefs::media_webm_enabled()) {
    return false;
  }

  MediaResult rv = NS_OK;
  auto tracks = GetTracksInfo(aContainerType, rv);

  if (NS_FAILED(rv)) {
    return false;
  }

  if (tracks.IsEmpty()) {
    // Matroska guarantees that the only codecs it contained are vp8, vp9, opus or
    // vorbis.
    return true;
  }

  // Verify that we have a PDM that supports the whitelisted types, include
  // color depth
  RefPtr<PDMFactory> platform = new PDMFactory();
  for (const auto& track : tracks) {
    if (!track ||
        platform
            ->Supports(SupportDecoderParams(*track), nullptr /* diagnostic */)
            .isEmpty()) {
      return false;
    }
  }

  return true;
}

/* static */
nsTArray<UniquePtr<TrackInfo>> MatroskaDecoder::GetTracksInfo(
    const MediaContainerType& aType) {
  MediaResult rv = NS_OK;
  return GetTracksInfo(aType, rv);
}

}  // namespace mozilla