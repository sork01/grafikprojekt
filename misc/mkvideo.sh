ffmpeg -r 60 -f image2 -s 640x480 -i image%d.bmp -vcodec libx264 -crf 5  -pix_fmt yuv420p test.mp4
