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
  -f [ --folder ] arg (=./)     folder of where to download content to
```

For example, to concurrently download files of type `mp3` from a given url:

```
./bigd --url <URL> --type mp3
```
 
Notes:

* Unless specified using the `--folder` flag, all content is downloaded to the current working directory.
* A threadpool is used to concurrently scrape content (so should prove quicker than tools like wget).
* The default threading value means that 10 files are simultenously downloaded, at a time. This can be overridden via the `--threads` flag.
* A history of downloaded content (a 'download archive') will be written to a file at a path specified by the `--download-archive` flag. 
* The download archive is also used to ensure that bigd doesn't attempt to re-download content already downloaded.
* Recursive downloading is supported with the `--depth` argument but is disabled by default (a depth of zero).

Tested on a bunch of open directories.

To compile, you need boost and libcurl, and then something like the following which works for me on mac:

```
clang++ -std=c++11 bigd.cpp -lcurl -lboost_program_options -o bigd
```

Please create an issue if you find a bug / have an enhancement request. NB. I've not extensively tested this.

Adheres to The Hacky As Fuck Software License.