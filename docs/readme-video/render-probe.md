# GitHub Markdown Render Probe

GitHub이 저장소 마크다운 파일(README 등)을 렌더링할 때 어떤 태그/패턴이 살아남는지
실측하기 위한 파일입니다. 결과는 `FEASIBILITY.md`에 정리되어 있습니다.

## 0. Control (대조군)

CONTROL_LINK_START [normal-link-survives](https://example.com/control) CONTROL_LINK_END

## 1. YouTube iframe (표준 임베드)

IFRAME_START<iframe width="560" height="315" src="https://www.youtube.com/embed/dQw4w9WgXcQ" title="YouTube video player" frameborder="0" allowfullscreen>IFRAME_INNER_TEXT</iframe>IFRAME_END

## 2. 외부 호스팅 video 태그

EXTVIDEO_START<video src="https://example.com/sample.mp4" controls width="480">EXTVIDEO_INNER_TEXT</video>EXTVIDEO_END

## 3. GitHub 첨부 자산 URL (bare autolink)

아래 줄은 GitHub 업로드 자산 형식의 URL 단독 줄입니다.

https://github.com/user-attachments/assets/00000000-1111-2222-3333-444444444444

## 4. GitHub 첨부 자산 + video 태그

VIDEOTAG_START<video src="https://github.com/user-attachments/assets/00000000-1111-2222-3333-444444444444" controls>VIDEOTAG_INNER_TEXT</video>VIDEOTAG_END

## 5. embed / object / audio

EMBED_START<embed src="https://example.com/a.mp4">EMBED_END
OBJECT_START<object data="https://example.com/a.mp4">OBJECT_INNER_TEXT</object>OBJECT_END
AUDIO_START<audio src="https://example.com/a.mp3" controls>AUDIO_INNER_TEXT</audio>AUDIO_END

## 6. script / style

SCRIPT_START<script>var SCRIPT_INNER_TEXT=1;</script>SCRIPT_END
STYLE_START<style>body{color:red}</style>STYLE_END

## 7. 썸네일 링크 패턴 (현재 사실상 표준)

[![YouTube thumbnail](https://img.youtube.com/vi/dQw4w9WgXcQ/hqdefault.jpg)](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

## 8. 외부 동적 SVG (camo 프록시 경유)

![typing svg](https://readme-typing-svg.demolab.com/?lines=animated+svg+works+on+github)

## 9. details 접기 + 이미지

<details>
<summary>DETAILS_SUMMARY_TEXT</summary>

DETAILS_BODY_TEXT

</details>
