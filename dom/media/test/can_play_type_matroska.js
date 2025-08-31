async function check_matroska(v, enabled) {
  function check(type, expected) {
    is(
      v.canPlayType(type),
      enabled ? expected : "",
      type + "='" + expected + "'"
    );
  }

  // Matroska types
  check("video/x-matroska", "maybe");
  check("audio/x-matroska", "maybe");
  check("video/mkv", "maybe");
  check("audio/mkv", "maybe");

  var video = ["vp8", "vp8.0", "vp9", "vp9.0", "avc1", "h264", "hvc1", "hevc"];
  var audio = ["vorbis", "opus", "mp4a", "aac"];

  audio.forEach(function (acodec) {
    check("audio/x-matroska; codecs=" + acodec, "probably");
    check("video/x-matroska; codecs=" + acodec, "probably");
    check("audio/mkv; codecs=" + acodec, "probably");
    check("video/mkv; codecs=" + acodec, "probably");
  });
  video.forEach(function (vcodec) {
    check("video/x-matroska; codecs=" + vcodec, "probably");
    check("video/mkv; codecs=" + vcodec, "probably");
    audio.forEach(function (acodec) {
      check('video/x-matroska; codecs="' + vcodec + ", " + acodec + '"', "probably");
      check('video/x-matroska; codecs="' + acodec + ", " + vcodec + '"', "probably");
      check('video/mkv; codecs="' + vcodec + ", " + acodec + '"', "probably");
      check('video/mkv; codecs="' + acodec + ", " + vcodec + '"', "probably");
    });
  });

  // Unsupported Matroska codecs
  check("video/x-matroska; codecs=xyz", "");
  check("video/x-matroska; codecs=xyz,vorbis", "");
  check("video/x-matroska; codecs=vorbis,xyz", "");
  check("video/mkv; codecs=xyz", "");
  check("video/mkv; codecs=xyz,vorbis", "");
  check("video/mkv; codecs=vorbis,xyz", "");

  await SpecialPowers.pushPrefEnv({ set: [["media.av1.enabled", true]] });
  check('video/x-matroska; codecs="av01"', "probably");
  check('video/mkv; codecs="av01"', "probably");

  await SpecialPowers.pushPrefEnv({ set: [["media.av1.enabled", false]] });
  check('video/x-matroska; codecs="av01"', "");
  check('video/mkv; codecs="av01"', "");
}