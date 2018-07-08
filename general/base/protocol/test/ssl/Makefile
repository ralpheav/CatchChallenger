all: hello.html

hello.html: hello.cpp ssl.cpp
	emcc -L../../lib/openssl_em/openssl-1.1.0h -I../../lib/openssl_em/openssl-1.1.0h/include hello.cpp ssl.cpp -s WASM=1 -o hello.html -lssl -lcrypto --emrun

clean:
	rm hello.html hello.wasm hello.js 
