// This 'ere software is hacky as fuck. Use at your peril.
// I'd appreciate a mention if you do. But if you don't,
// I'm not going to lose sleep over it.
// Ben Jones in the year of 2018.

#include <boost/program_options.hpp>
#include <curl/curl.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>

#include <stdio.h>

namespace detail {

    size_t find_nth(std::string const & haystack, 
                    size_t pos, std::string const & needle, size_t nth) 
    {
        size_t found_pos = haystack.find(needle, pos);
        if(0 == nth || std::string::npos == found_pos)  return found_pos;
        return find_nth(haystack, found_pos+1, needle, nth-1);
    }

    // A very basic URL type
    struct URL {

        explicit URL(std::string url)
        {
            // find third slash
            auto found = find_nth(url, 0, "/", 2);
            if(found == -1) {
                throw std::runtime_error("No path component!");
            }
            m_base.assign(std::begin(url), std::begin(url) + found);
            m_path.assign(std::begin(url) + found, std::end(url));
        }

        URL(std::string const & base,
            std::string const & path)
        : m_base(base)
        , m_path(path)
        {
        }

        void addPathBit(std::string const & path)
        {
            // Check if absolute. If so, should replace
            // while path
            if(path[0] == '/') {
                m_path = path;
            } 
            // Relative, just do a simple append
            else {
                if(m_path.back() != '/') {
                    m_path = m_path + "/" + path;
                } else {
                    m_path = m_path + path;
                }
            }
        }

        std::string getBase() const
        {
            return m_base;
        }

        std::string getPath() const
        {
            return m_path;
        }

        std::string getFilename() const
        {
            auto found = m_path.find_last_of("/");
            return {std::begin(m_path) + found + 1, std::end(m_path)};
        }

        std::string getWholeThing() const
        {
            return m_base + m_path;
        }

      private:
        std::string m_base;
        std::string m_path;


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

    void download_all(std::vector<URL> const & links,
                      int const nThreads)
    {

        std::vector<std::thread> downloadThreads;
        std::atomic<long> counter(0);

        for(auto const & it : links) {

            // While threads not exhausted, created a
            // new download thread
            if(downloadThreads.size() < nThreads) {
                downloadThreads.emplace_back([=]{
                    std::ofstream file(it.getFilename(), std::ios::binary);
                    pull_one_url(it.getWholeThing(), &file);
                    file.close();
                    std::cout<<"="<<std::flush;

                });
                ++counter;
            } 
            // Join on threads
            else if (downloadThreads.size() == nThreads || 
                     counter == links.size() - 1) {
                for(auto & t : downloadThreads) {
                    t.join();
                }
                downloadThreads.clear();
                std::cout<<std::endl;
            }
        }
    }

    bool hasType(std::string const & toCheck,
                 std::vector<std::string> const & types)
    {
        for(auto const & type : types) {
            if(toCheck.find(type) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

}

int main(int argc, char *argv[]) {
    
    // Webpage to download from
    std::string url;

    // Type of file to download
    std::vector<std::string> types;

    // Number of files to simul. download
    int threads;

    // Parse nput arguments
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("url,u", po::value<std::string>(&url), "page to download from")
        ("type,t", po::value<std::vector<std::string>>(&types), "type of file to download")
        ("threads,n", po::value<int>(&threads)->default_value(10), "number of files to simultaneously download")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm); 

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (!vm.count("url")) {
        std::cout<<desc<<std::endl;
        return 1;
    }

    if (!vm.count("type")) {
        std::cout<<desc<<std::endl;
        return 1;
    }

    // auto url = vm.count("url");
    if(url.back() != '/') {
        url.push_back('/');
    }

    // Extract all links from page at url
    std::ostringstream stream;
    detail::pull_one_url(url, &stream);
    auto const links = detail::extract_hyperlinks(stream.str());

    // The base URL representing webpage
    detail::URL baseURL(url);

    // Process links having particular extension
    std::vector<detail::URL> processed;
    for (auto const & it : links) {
        if(detail::hasType(it, types)) {

            // Find filename part, it multiple
            // path parts. Ignore parent parts.
            // The filename will be the final part
            // after the final slash.
            auto urlCopy = baseURL;
            urlCopy.addPathBit(it);
            processed.push_back(urlCopy);
        }
    }

    // Now do multi-threaded download of all links
    detail::download_all(processed, threads);

    return 0;
}