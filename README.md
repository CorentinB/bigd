# bigd : a concurrent file downloader

This is a tool that you can use to concurrently scrape files from a webpage. Usage:

```
./bigd --url <URL> --type mp3 --threads 5
```

will download all files of type mp3 from url `<URL>` and will concurrently download a maximum of 5 at any given moment.

There is probably a bit more scope to optimize here but hopefully its simpler than trying to get a tool like `wget` to concurrently download.

Tested on a bunch of open directories.

To compile, you need boost and libcurl, then for me on my mac:

```
clang++ -std=c++11 bigd.cpp -lcurl -lboost_program_options -o bigd
```