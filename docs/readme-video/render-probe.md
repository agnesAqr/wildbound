# GitHub Markdown Render Probe v2

각 줄은 `[[ 마커 ]]` 사이에 HTML을 넣어, GitHub 렌더링 후 무엇이 남는지 측정합니다.

L1 iframe: [[ <iframe src="https://www.youtube.com/embed/dQw4w9WgXcQ" width="560" height="315">AAA_IFRAME_INNER</iframe> ]] end-L1

L2 external-video: [[ <video src="https://example.com/sample.mp4" controls>BBB_EXTVIDEO_INNER</video> ]] end-L2

L3 embed: [[ <embed src="https://example.com/a.mp4"> ]] end-L3

L4 object: [[ <object data="https://example.com/a.mp4">CCC_OBJECT_INNER</object> ]] end-L4

L5 audio: [[ <audio src="https://example.com/a.mp3" controls>DDD_AUDIO_INNER</audio> ]] end-L5

L6 script: [[ <script>var EEE_SCRIPT_INNER=1;</script> ]] end-L6

L7 style: [[ <style>body{color:red}</style> ]] end-L7

L8 control-link: [[ [FFF_CONTROL_LINK](https://example.com/control) ]] end-L8

L9 github-hosted video tag:

<video src="https://github.com/user-attachments/assets/5cab4668-e8e2-4982-8551-aab05ff73a7f" controls width="480">GGG_VIDEOTAG_INNER</video>

L10 github-hosted bare url:

https://github.com/user-attachments/assets/5cab4668-e8e2-4982-8551-aab05ff73a7f

L11 youtube thumbnail link:

[![thumb](https://img.youtube.com/vi/dQw4w9WgXcQ/hqdefault.jpg)](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

L12 animated svg via camo:

![typing svg](https://readme-typing-svg.demolab.com/?lines=animated+svg+works)
