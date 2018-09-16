# bigd : a concurrent file downloader

This is a command-line tool that you can use to concurrently scrape files from a webpage.

Usage examples are based on the following options:

```
Allowed options:
  -h [ --help ]                 produce help message
  -u [ --url ] arg              page to download from
  -t [ --type ] arg             type of file to download
  -n [ --threads ] arg (=10)    number of files to simultaneously download
  -d [ --depth ] arg (=0)       recursive depth
  -a [ --download-archive ] arg archive file path
```

For example:

```
./bigd --url <URL> --type mp3
```

will concurrently download all files of type mp3 from url `<URL>`. All files are dumped to the current working directory.

In addition, a threadpool is used to download `threads` number of files at any given moment.
Hopefully therefore, bigd should prove quicker than tools like wget.

Finally, recursive downloading is also supported with the `--depth` argument. By default,
recursive downloading is disabled, as signified by a 'depth' value of zero.

Tested on a bunch of open directories.

To compile, you need boost and libcurl, and then something like the following which works for me on mac:

```
clang++ -std=c++11 bigd.cpp -lcurl -lboost_program_options -o bigd
```

Please create an issue if you find a bug / have an enhancement request. NB. I've not extensively tested this.

Adheres to The Hacky As Fuck Software License.