# bigd : a concurrent file downloader

This is a command-line tool that you can use to concurrently scrape files from a webpage. Usage:

```
./bigd --url <URL> --type mp3 --threads 5
```

will download all files of type mp3 from url `<URL>`. In addition, a threadpool is used to download `threads` 
number of files at any given moment. Hopefully therefore, bigd should prove quicker than tools like wget.

Tested on a bunch of open directories.

Note: all files are dumped to the current working directory.

To compile, you need boost and libcurl, and then something like the following which works for me on mac:

```
clang++ -std=c++11 bigd.cpp -lcurl -lboost_program_options -o bigd
```

Please create an issue if you find a big / have an enhacement request. NB. I've not extensively tested this.

Adheres to The Hacky As Fuck Software License.