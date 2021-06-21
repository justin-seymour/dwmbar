
dwmbar: dwmbar.c
	gcc -g -Wall -o $@ $< -lX11

clean:
	rm dwmbar
