#include "web_crawler.h"

#include "web_utils.h"
#include "web_crawler_config.h"
#include "content_formatting.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <curl/curl.h>

// REQUIRES: url encoded google search query, maximum length, status code address
// EFFECTS: execute google search api call, structure api response, return as JSON formatted string
char* get_content_list(const char* query, int* status_code, size_t max_content_length) {
    char* content_list = NULL;
    CURL* curl_handle = create_curl_handle();
    if (!curl_handle) {
        goto destroy_curl_return;
    }

    load_env("..\\.env");
    char* search_url = get_google_search_url(query);
    if (!search_url) {
        goto destroy_curl_return;
    }

    char* webpage_content = fetch_webpage_content(search_url, status_code, curl_handle, NULL);
    free(search_url);
    if (!webpage_content) {
        goto destroy_curl_return;
    }

    content_list = structure_google_query_response(webpage_content, max_content_length);
    free(webpage_content);

destroy_curl_return:
    destroy_curl_handle(curl_handle);
    return content_list;
}

// REQUIRES: url, status code address
// EFFECTS: Looks through webpage in the url, returns relevant information as JSON formatted string
char* get_content_item(const char* url, int* status_code, int* escaped, size_t max_content_length) {
    char* content_json = NULL;
    CURL* curl_handle = create_curl_handle();
    if (!curl_handle) {
        goto destroy_curl_return;
    }
    struct curl_slist* headers = create_curl_headers();

    WebsiteType website_type = detect_website_type(url);

    printf("website_type: %d\n", (int)website_type);
    char* new_url = web_specific_setup(url, website_type, curl_handle, &headers, escaped);
    if (!new_url) {
        goto destroy_curl_return;
    }

    printf("new_url: %s\n", new_url);

    // To avoid spamming stackoverflow api
    char* webpage_content = "{\"items\":[{\"owner\":{\"account_id\":1932184,\"reputation\":554,\"user_id\":1741355,\"user_type\":\"registered\",\"profile_image\":\"https://i.sstatic.net/Fh2mU.png?s=256\",\"display_name\":\"Aaron S. Kurland\",\"link\":\"https://stackoverflow.com/users/1741355/aaron-s-kurland\"},\"is_accepted\":true,\"score\":42,\"last_activity_date\":1533742293,\"last_edit_date\":1533742293,\"creation_date\":1389216126,\"answer_id\":21006798,\"question_id\":21006707,\"content_license\":\"CC BY-SA 4.0\",\"body\":\"<p>Just don't call <code>free()</code> on your original ptr in the happy path. Essentially <code>realloc()</code> has done that for you.</p>\\n\\n<pre><code>ptr = malloc(sizeof(int));\\nptr1 = realloc(ptr, count * sizeof(int));\\nif (ptr1 == NULL) // reallocated pointer ptr1\\n{       \\n    printf(\\\"\\\\nExiting!!\\\");\\n    free(ptr);\\n    exit(0);\\n}\\nelse\\n{\\n    ptr = ptr1;           // the reallocation succeeded, we can overwrite our original pointer now\\n}\\n</code></pre>\\n\"},{\"owner\":{\"account_id\":3824066,\"reputation\":2148,\"user_id\":3171324,\"user_type\":\"registered\",\"profile_image\":\"https://www.gravatar.com/avatar/4419649e67e4adb3b6a8ff41fd82bad4?s=256&d=identicon&r=PG&f=y&so-version=2\",\"display_name\":\"Keeler\",\"link\":\"https://stackoverflow.com/users/3171324/keeler\"},\"is_accepted\":false,\"score\":5,\"last_activity_date\":1504649613,\"last_edit_date\":1504649613,\"creation_date\":1389218157,\"answer_id\":21007414,\"question_id\":21006707,\"content_license\":\"CC BY-SA 3.0\",\"body\":\"<p>Applying fixes as edits, based on the good comments below.</p>\\n\\n<p>Reading <a href=\\\"http://www.c-faq.com/malloc/realloc.html\\\" rel=\\\"nofollow noreferrer\\\">this comp.lang.c question</a>, reveals 3 cases:</p>\\n\\n<ol>\\n<li>\\\"When it is able to, it simply gives you back the same pointer you handed it.\\\"</li>\\n<li>\\\"But if it must go to some other part of memory to find enough contiguous space, it will return a different pointer (and the previous pointer value will become unusable).\\\"</li>\\n<li>\\\"If <code>realloc</code> cannot find enough space at all, it returns a null pointer, and leaves the previous region allocated.\\\"</li>\\n</ol>\\n\\n<p>This can be translated directly to code:</p>\\n\\n<pre><code>int* ptr = (int*)malloc(sizeof(int));\\nint* tmp = (int*)realloc(ptr, count * sizeof(int));\\nif(tmp == NULL)\\n{\\n    // Case 3, clean up then terminate.\\n    free(ptr);\\n    exit(0);\\n}\\nelse if(tmp == ptr)\\n{\\n    // Case 1: They point to the same place, so technically we can get away with\\n    // doing nothing.\\n    // Just to be safe, I'll assign NULL to tmp to avoid a dangling pointer.\\n    tmp = NULL;\\n}\\nelse\\n{\\n    // Case 2: Now tmp is a different chunk of memory.\\n    ptr = tmp;\\n    tmp = NULL;\\n}\\n</code></pre>\\n\\n<p>So, if you think about it, the code you posted is fine (almost). The above code simplifies to:</p>\\n\\n<pre><code>int* ptr = (int*)malloc(sizeof(int));\\nint* tmp = (int*)realloc(ptr, count * sizeof(int));\\nif(tmp == NULL)\\n{\\n    // Case 3.\\n    free(ptr);\\n    exit(0);\\n}\\nelse if(ptr != tmp)\\n{\\n    ptr = tmp;\\n}\\n// Eliminate dangling pointer.\\ntmp = NULL;\\n</code></pre>\\n\\n<p>Note the extra <code>else if(ptr != tmp)</code>, which excludes Case 1, where you wouldn't want to call <code>free(ptr)</code> because <code>ptr</code> and <code>tmp</code> refer to the same location. Also, just for safety, I make sure to assign <code>NULL</code> to <code>tmp</code> to avoid any dangling pointer issues while <code>tmp</code> is in scope.</p>\\n\"},{\"owner\":{\"account_id\":45619,\"reputation\":124765,\"user_id\":134554,\"user_type\":\"registered\",\"accept_rate\":0,\"profile_image\":\"https://i.sstatic.net/1qpPd.png?s=256\",\"display_name\":\"John Bode\",\"link\":\"https://stackoverflow.com/users/134554/john-bode\"},\"is_accepted\":false,\"score\":0,\"last_activity_date\":1389220792,\"creation_date\":1389220792,\"answer_id\":21008101,\"question_id\":21006707,\"content_license\":\"CC BY-SA 3.0\",\"body\":\"<p>You should <em>not</em> <code>free</code> your original pointer if the <code>realloc</code> succeeds.  Whether you <code>free</code> that pointer if the <code>realloc</code> fails depends on the needs of your particular application; if you <em>absolutely cannot continue</em> without that additional memory, then this would be a fatal error and you would deallocate any held storage and exit.  If, OTOH, you can still continue (perhaps execute a different operation and hope that memory will come available later), the you'd probably want to hold on to that memory and a attempt a another <code>realloc</code> later.  </p>\\n\\n<p><a href=\\\"http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf\\\" rel=\\\"nofollow\\\">Chapter and verse</a>:</p>\\n\\n<blockquote>\\n<p><b>7.22.3.5 The realloc function</b></p>\\n<p><b>Synopsis</b></p>\\n1<pre><code>     #include &lt;stdlib.h&gt;\\n     void *realloc(void *ptr, size_t size);</code></pre>\\n<p><b>Description</b></p>\\n<p>2 The <code>realloc</code> function <b>deallocates the old object</b> pointed to by <code>ptr</code> and returns a\\npointer to a new object that has the size speci´¼üed by <code>size</code>. The contents of the new\\nobject shall be the same as that of the old object prior to deallocation, up to the lesser of\\nthe new and old sizes. Any bytes in the new object beyond the size of the old object have\\nindeterminate values.</p>\\n<p>3 If <code>ptr</code> is a null pointer, the <code>realloc</code> function behaves like the <code>malloc</code> function for the\\nspeci´¼üed size. Otherwise, if <code>ptr</code> does not match a pointer earlier returned by a memory\\nmanagement function, or if the space has been deallocated by a call to the <code>free</code> or\\n<code>realloc</code> function, the behavior is unde´¼üned. <b>If memory for the new object cannot be\\nallocated, the old object is not deallocated and its value is unchanged.</b></p>\\n<p><b>Returns</b></p>\\n<p>4 The <code>realloc</code> function returns a pointer to the new object (which may have the same\\nvalue as a pointer to the old object), or a null pointer if the new object could not be\\nallocated.</p>\\n</blockquote>\\n\\n<p>Emphasis added.  Note clause 4; the returned pointer may be the same as your original pointer. </p>\\n\"},{\"owner\":{\"account_id\":491331,\"reputation\":29924,\"user_id\":1401900,\"user_type\":\"registered\",\"accept_rate\":60,\"profile_image\":\"https://i.sstatic.net/fCVY7.jpg?s=256\",\"display_name\":\"Paulo Bu\",\"link\":\"https://stackoverflow.com/users/1401900/paulo-bu\"},\"is_accepted\":false,\"score\":1,\"last_activity_date\":1389216580,\"last_edit_date\":1389216580,\"creation_date\":1389216183,\"answer_id\":21006814,\"question_id\":21006707,\"content_license\":\"CC BY-SA 3.0\",\"body\":\"<p><code>realloc</code> will return the same address to <code>ptr</code> if it have enough space to extend the actual chunk of memory pointed by <code>ptr</code>. Otherwise, it will move the data to the new chunk and free the old chunk. You can not rely on <code>ptr1</code> being different to <code>ptr</code>. Your program behaves undefined.</p>\\n\\n<p>If <code>realloc</code> returns another address, it first deallocates the old one so you don't have to do it yourself.</p>\\n\\n<p>By the way, never cast the return of <code>malloc/realloc</code> :). Your code should be like this:</p>\\n\\n<pre><code>ptr=malloc(sizeof(int));\\nptr=realloc(ptr,count*sizeof(int));\\nif(ptr==NULL)\\n{   \\n    // error!    \\n    printf(\\\"\\\\nExiting!!\\\");\\n    // no need to free, the process is exiting :)\\n    exit(0);\\n}\\n</code></pre>\\n\"},{\"owner\":{\"account_id\":2802349,\"reputation\":158568,\"user_id\":2410359,\"user_type\":\"registered\",\"accept_rate\":87,\"profile_image\":\"https://i.sstatic.net/pIl9T.png?s=256\",\"display_name\":\"chux\",\"link\":\"https://stackoverflow.com/users/2410359/chux\"},\"is_accepted\":false,\"score\":2,\"last_activity_date\":1389216439,\"last_edit_date\":1492087236,\"creation_date\":1389216119,\"answer_id\":21006795,\"question_id\":21006707,\"content_license\":\"CC BY-SA 3.0\",\"body\":\"<p>OP: ... may be different from ptr, or NULL if the request fails.<br>\\nA: Not always.  <code>NULL</code> may be legitimately returned (not a failure), if <code>count</code> is 0.</p>\\n\\n<p>OP: Is it sufficient to just assume that the reallocated pointer points to a different block of memory and not to the same block.<br>\\nA: No</p>\\n\\n<p>OP: Should I put in another condition which will compare the equality of ptr and ptr1 and exclude the execution of the free(ptr) statement?<br>\\nA: No.</p>\\n\\n<p>If <code>realloc()</code> returns <code>NULL</code> (and count is not 0), the value of <code>ptr</code> is still valid, pointing to the un-resized data.  <code>free(ptr)</code> or not depends on your goals.  </p>\\n\\n<p>If <code>realloc()</code> returns not <code>NULL</code>, do not <code>free(ptr)</code>, it is all ready freed.</p>\\n\\n<p>Example: <a href=\\\"https://codereview.stackexchange.com/questions/36662/critique-of-realloc-wrapper\\\">https://codereview.stackexchange.com/questions/36662/critique-of-realloc-wrapper</a></p>\\n\\n<pre><code>#include &lt;assert.h&gt;\\n#include &lt;stdlib.h&gt;\\n\\nint ReallocAndTest(char **Buf, size_t NewSize) {\\n  assert(Buf);\\n  void *NewBuf = realloc(*Buf, NewSize);\\n  if ((NewBuf == NULL) &amp;&amp; (NewSize &gt; 0)) {\\n    return 1;  // return failure\\n  }\\n  *Buf = NewBuf;\\n  return 0;\\n}\\n</code></pre>\\n\"},{\"owner\":{\"account_id\":3770419,\"reputation\":1761,\"user_id\":3132801,\"user_type\":\"registered\",\"profile_image\":\"https://www.gravatar.com/avatar/3e40228d0c1c0f717a17bc54e6ceec9c?s=256&d=identicon&r=PG&f=y&so-version=2\",\"display_name\":\"tabstop\",\"link\":\"https://stackoverflow.com/users/3132801/tabstop\"},\"is_accepted\":false,\"score\":0,\"last_activity_date\":1389216111,\"creation_date\":1389216111,\"answer_id\":21006791,\"question_id\":21006707,\"content_license\":\"CC BY-SA 3.0\",\"body\":\"<p>If <code>realloc</code> moves your data, it will free the old pointer for you behind the scenes. I don't have a copy of the C11 standard, but it is guaranteed in the C99 standard.</p>\\n\"}],\"has_more\":false,\"quota_max\":300,\"quota_remaining\":217}";
    free(new_url);

    content_json = structure_webpage_content_response(webpage_content, website_type, max_content_length);

    // Actual ver
    // char* webpage_content = fetch_webpage_content(new_url, status_code, curl_handle, headers);
    // free(new_url);
    // if (!webpage_content) {
    //     goto destroy_curl_return;
    // }

    // content_json = structure_webpage_content_response(webpage_content, website_type, max_content_length);
    // free (webpage_content);

destroy_curl_return:
    destroy_curl_handle(curl_handle);
    return content_json;
}