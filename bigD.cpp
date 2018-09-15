// Hack to download all files of given extension on webpage

#include <stdio.h>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>

struct LinkDetail {
    std::string base;
    std::string filename;
};

size_t write_data(const char * data, size_t size, size_t count, std::ostream * stream) {
    stream->write(data, count);
    return size * count;
}

std::set<std::string> extract_hyperlinks(std::string const & text)
{
    static std::regex const hl_regex( "<a href=\"(.*?)\">", std::regex_constants::icase  ) ;
    return { std::sregex_token_iterator( text.begin(), text.end(), hl_regex, 1 ),
             std::sregex_token_iterator{} } ;
}

void pull_one_url(std::string const & url,
                  std::ostream * stream)
{
    CURL *curl;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stream);
        curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        stream->flush();
    }
}

void download_all(std::vector<LinkDetail> const & links)
{

    std::vector<std::thread> downloadThreads;
    int const nThreads = 10;

    for(auto const & it : links) {

        // While threads not exhausted, created a
        // new download thread
        if(downloadThreads.size() < nThreads) {
            downloadThreads.emplace_back([=]{
                std::ofstream file(it.filename, std::ios::binary);
                pull_one_url(it.base + it.filename, &file);
                file.close();
                std::cout<<it.filename<<" done!"<<std::endl;
            });
        } 
        // Join on threads
        else {
            for(auto & t : downloadThreads) {
                t.join();
            }
            downloadThreads.clear();
        }
    }
}

int main(int argc, char *argv[]) {
    
    // Extract all links from a given webpage
    std::string url(argv[1]);
    if(url.back() != '/') {
        url.push_back('/');
    }
    std::ostringstream stream;
    pull_one_url(url, &stream);
    auto const links = extract_hyperlinks(stream.str());

    // Process links having particular extension
    std::vector<LinkDetail> processed;
    for (auto const & it : links) {
        if(it.find(argv[2]) != std::string::npos) {
            LinkDetail ld{url, it};
            processed.push_back(ld);
        }
    }

    // Now do multi-threaded download of all links
    download_all(processed);

    return 0;
}