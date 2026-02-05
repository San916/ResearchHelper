# ResearchHelper (C) (Updated Feb 5, 2026)
A web app made to help conduct online research at a faster pace. Web server made in C with CMake, using libcurl for web crawling, and Unity for testing. Frontend built with base html, css, js.

# Features
* Make code-related queries and receive formatted results from multiple webpages, including stackexchange and reddit forums
* Change the minimum score in answers coming from forums to include only the answers that other users trust
* Toggle to only view code snippets
* Save queries to revisit later

<h3>Why these features?</h3> When I'm working on a problem, my research usually goes like this: I'll type in a question on google, ctrl + click the top several links, then look through each page one by one. I'd have to sift through irrelevant text and code until I find a good answer. I wanted a way to increase my quality of life during this process. With the additions of constraints, such as minimum score, code only, etc, I'm able to cut down on time I spend sifting through irrelevant answers. With everything in one place and with queries being stored, I'm not required to constantly open and close new tabs, then keep a tab up in fear of having to use it at some point, etc.

<img src="github_demo/demo_1.gif">
<img src="github_demo/demo_2.gif">

# Technologies Used
* Web server made in C with winsock, supporting Http/1.1
* Web crawling with libcurl
* Unit/Integration tests made with Unity, a lightweight C testing library 
* CMake to automate building
* Project managed with Jira

<h3>Why these technologies?</h3>
Why a web server in C? I recently took CPSC 317 at UBC, and wanted to get a more intimate view of HTTP and web servers without the abstractions present in other technologies. I also just thought it was a good opportunity to revisit C after not having touched it in a while.

For frontend, since it's very basic, I ended up not using libraries to avoid relying on tons of dependencies.

With testing, I used Unity as it's really small and I could just copy the source code directly into my project. Honestly I feel like I could've gotten away with not using it.

I feel similarly with libcurl as I do with Unity. I think it would've been an interesting challenge to try implement my web crawling without the use of libcurl.

# How to Use (Windows)
* Install libcurl(8.17.0) with vcpkg
* Clone repo, and create a .env file containing keys for: google search api and search engine, reddit user (for api)
```
GOOGLE_SEARCH_API_KEY=XXX
GOOGLE_SEARCH_ENGINE=XXX
REDDIT_ID=u/XXX
```
* Create a `/build` directory in the project folder and `cd` into it
* Run:
```
cmake ..
cmake --build .
```
* To start server:
```
debug\app
```
* For tests, run:
```
ctest -C debug 
```
* Individual tests:
```
ctest -C debug -R debug\invidivual_test_name 
```