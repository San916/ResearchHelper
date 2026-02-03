document.addEventListener("DOMContentLoaded", function() {
    const mainPage = document.getElementById("main-page");
    const footer = document.getElementById("footer");
    mainPage.style.display = "none";

    const mainPanel = document.getElementById("main-panel");
    mainPanel.addEventListener("wheel", function (event) {
        const inner = event.target.closest(".result-content");
        if (inner) return;

        const response = document.getElementById("response");
        const responseContent = response.querySelector(".response-content")
        const delta = event.deltaY * 0.5;
        const maxScroll = responseContent.scrollHeight - responseContent.clientHeight;

        responseContent.scrollTop = Math.min(Math.max(responseContent.scrollTop + delta, 0), maxScroll);
        event.preventDefault();
    }, { passive: false });
});

const viewSettings = {
    codeOnly: false,
    showOriginalPost: true
};

document.addEventListener("DOMContentLoaded", function() {
    document.querySelectorAll(".toggle-able").forEach((item) => {
        item.addEventListener("click", function() {            
            this.classList.toggle("selected");

            if (this.id === "code-only-toggle") {
                viewSettings.codeOnly = this.classList.contains("selected");
            } else if (this.id === "show-original-post-toggle") {
                viewSettings.showOriginalPost = this.classList.contains("selected");
            }

            reRenderContent();
        });
    });
});

function reRenderContent() {
    document.querySelectorAll(`.result-content[data-loaded="true"]`).forEach(contentArea => {
        const url = contentArea.dataset.url;
        const params = JSON.parse(contentArea.dataset.params);
        const savedContent = webpageContentStorage.checkSavedWebContent(url, params);
        if (savedContent) {
            displayWebContent(contentArea, savedContent.content, savedContent.escaped, url, params);
        }
    });
}

function displayResults(data, query) {
    const responseDiv = document.getElementById("response");

    responseDiv.innerHTML = "";

    const responseHeader = document.createElement("div");
    responseHeader.className = "response-header";
    const responseText = document.createElement("h2");
    responseText.innerText = `Results for: ${query}`;
    responseHeader.appendChild(responseText);
    responseDiv.appendChild(responseHeader);

    if (!data.results || data.results.length === 0) {
        responseDiv.innerHTML += `<div class="no-results">No results!</div>`;
        return;
    }

    const responseContent = document.createElement("div");
    responseContent.className = "response-content";

    data.results.forEach((result) => {
        const resultElement = document.createElement("div");
        resultElement.className = "result-item";
        
        const resultExpander = document.createElement("div");
        resultExpander.className = "result-expander";
        resultExpander.textContent = "▼";

        const resultHeader = document.createElement("div");
        resultHeader.className = "result-header";
        resultHeader.appendChild(resultExpander);
        resultHeader.innerHTML += `
            <a href="${result.link}" target="_blank" rel="noopener noreferrer">
                <h3 class="result-title">${result.title}</h3>
            </a>
        `;

        const resultContent = document.createElement("div");
        resultContent.className = "result-content";
        resultContent.style.display = "none";
        
        resultHeader.addEventListener("click", async function() {
            const visible = resultContent.style.display !== "none";
            resultContent.style.display = "none";
            if (visible) {
                resultContent.style.display = "none";
                resultExpander.textContent = "▼";
            } else {
                resultContent.style.display = "block";
                resultExpander.textContent = "▲";
                
                if (!resultContent.dataset.loaded) {
                    await fetchAndDisplayContent(result.link, resultContent);
                }
            }
        });

        resultElement.appendChild(resultHeader);
        resultElement.appendChild(resultContent);
        responseContent.appendChild(resultElement);
    });
    
    responseDiv.appendChild(responseContent);
}

function extractCodeBlocks(html) {
    const tempDiv = document.createElement("div");
    tempDiv.innerHTML = html;
    
    const codeBlocks = tempDiv.querySelectorAll("pre");
    if (codeBlocks.length === 0) {
        return "No Code!";
    }
    
    let codeOnly = "";
    codeBlocks.forEach((element) => {
        codeOnly += element.outerHTML;
    });
    
    return codeOnly;
}

function displayWebContent(contentArea, jsonData, escaped, url, params) {
    function addItem(item, original_post) {
        let  html = item.content_body;

        if (escaped) {
            const txt = document.createElement("textarea");
            txt.innerHTML = html;
            html = txt.value;
        }

        if (viewSettings.codeOnly) {    
            html = extractCodeBlocks(html);
            if (!html.trim()) {
                return;
            }
        }

        const contentItem = document.createElement("div");
        contentItem.className = "content-item";

        const contentText = document.createElement("div");
        contentText.className = "content-text";
        contentText.innerHTML = html;

        const contentHeader = document.createElement("div");
        contentHeader.className = "content-header";
        if (!original_post) {
            contentHeader.innerHTML = `Score: ${item.score >= 0 ? item.score : "N/A"}`;
        } else {
            contentHeader.innerHTML = `Original Post:`;
        }

        const commentExpander = document.createElement("div");
        commentExpander.className = "comment-expander";
        commentExpander.textContent = "[close]";
        contentHeader.addEventListener("click", function() {
            const closing = commentExpander.textContent === "[close]";
            if (closing) {
                commentExpander.textContent = "[expand]";
                contentText.style.display = "none";
            } else {
                commentExpander.textContent = "[close]";
                contentText.style.display = "block";
            }
        });

        const invisibleBorder = document.createElement("div");
        invisibleBorder.className = "invisible-border";
        invisibleBorder.addEventListener("click", function() {
            contentHeader.click();
        })

        contentHeader.appendChild(commentExpander);

        contentItem.appendChild(contentHeader);
        contentItem.appendChild(contentText);
        contentItem.appendChild(invisibleBorder);

        contentArea.appendChild(contentItem);
    }

    contentArea.innerHTML = "";
    if (viewSettings.showOriginalPost) {
        addItem(jsonData.original_post, true);
    }
    jsonData.comments.forEach((item) => {
        addItem(item, false);
    });
    contentArea.dataset.loaded = true;
    contentArea.dataset.url = url;
    contentArea.dataset.params = JSON.stringify(params);
}

async function fetchAndDisplayContent(resultLink, contentArea) {
    try {
        let maxNumComments = document.getElementById("max-num-comments");
        let maxNumCommentsValue = maxNumComments.value;
        if (maxNumCommentsValue.length === 0) {
            maxNumCommentsValue = maxNumComments.placeholder;
        } else if (!Number.isFinite(Number(maxNumCommentsValue))) {
            maxNumComments.value = "";
            return;
        } else {
            maxNumCommentsValue = Number(maxNumComments.value);
        }

        let minScore = document.getElementById("min-score");
        let minScoreValue = minScore.value;
        if (minScoreValue.length === 0) {
            minScoreValue = minScore.placeholder;
        } else if (!Number.isFinite(Number(minScoreValue))) {
            minScore.value = "";
            return;
        } else {
            minScoreValue = Number(minScore.value);
        }

        const headers = {
            "Max-Num-Comments": maxNumCommentsValue,
            "Min-Score": minScoreValue
        };
        const savedContent = webpageContentStorage.checkSavedWebContent(resultLink, headers);
        if (savedContent) {
            displayWebContent(contentArea, savedContent.content, savedContent.escaped, resultLink, headers);
            return;
        }

        const response = await fetch(`/content?url=${encodeURIComponent(resultLink)}`, {
            method: "GET",
            headers: headers,
        });

        if (response.ok) {
            const escaped = response.headers.get("Html-Escaped") === "true";
            const data = await response.text();
            const jsonData = JSON.parse(data);
            displayWebContent(contentArea, jsonData, escaped, resultLink, headers);
            webpageContentStorage.saveWebContent(resultLink, headers, escaped, jsonData);
        } else {
            contentArea.textContent = `Error: ${response.status}`;
        }
    } catch (error) {
        contentArea.textContent = `Error: ${error.message}`;
        console.error("Error:", error);
    }
}

const queryStorage = {
    sessionStorageKey: "research_helper_query_history",

    getHistory: function() {
        const history = sessionStorage.getItem(this.sessionStorageKey);
        return history ? JSON.parse(history) : [];
    },

    saveQuery: function(query, response, id) {
        const history = this.getHistory();

        const entry = {
            query,
            response,
            id
        };
        history.unshift(entry);
          
        sessionStorage.setItem(this.sessionStorageKey, JSON.stringify(history));
    },

    clearHistory: function() {
        sessionStorage.removeItem(this.sessionStorageKey);
        const queryHistory = document.getElementById("query-history");
        queryHistory.innerHTML = "";
    },

    renderHistory: function(query, id) {
        const history = this.getHistory();

        const queryHistory = document.getElementById("query-history");
        if (history.length === 0) {
            queryHistory.innerHTML = "";
            return;
        }

        const newItem = document.createElement("div");
        newItem.className = "query-history-item";
        newItem.innerText = `${query}`;
        newItem.id = `${id}`;

        const currentlySelected = queryHistory.querySelector(".selected");
        if (currentlySelected) {
            currentlySelected.classList.remove("selected");
        }
        newItem.classList.add("selected");

        newItem.addEventListener("click", function (event) {
            const itemInfo = history.find((item) => (item.id === id));
            if (!itemInfo) {
                return;
            }
            const currentlySelected = queryHistory.querySelector(".selected");
            if (currentlySelected.id === event.target.id) {
                return;
            }
            currentlySelected.classList.remove("selected");
            event.target.classList.add("selected");
            displayResults(itemInfo.response, itemInfo.query);
        });
        queryHistory.prepend(newItem);
    }
}

const webpageContentStorage = {
    sessionStorageKey: "research_helper_web_content",
    
    getSavedWebContent: function() {
        const content = sessionStorage.getItem(this.sessionStorageKey);
        return content ? JSON.parse(content) : [];
    },

    checkSavedWebContent: function(url, params) {
        const content = this.getSavedWebContent();
        if (!content.length) {
            return null;
        }

        const savedContent = content.find((item) => {
            return (item.url === url && JSON.stringify(item.params) === JSON.stringify(params));
        });
        if (!savedContent) {
            return null;
        }
        return { content: savedContent.webContent, escaped: savedContent.escaped };
    },

    saveWebContent: function(url, params, escaped, webContent) {
        const content = this.getSavedWebContent();
        const entry = {
            url,
            params,
            escaped,
            webContent
        };
        content.unshift(entry);
          
        sessionStorage.setItem(this.sessionStorageKey, JSON.stringify(content));
    },

    clearWebContent: function() {
        sessionStorage.removeItem(this.sessionStorageKey);
    }
}

document.addEventListener("DOMContentLoaded", function() {
    const userInputForm = document.getElementById("user-input-form");
    const responseDiv = document.getElementById("response");
    const mainPage = document.getElementById("main-page");
    const titlePage = document.getElementById("title-page");
    const footer = document.getElementById("footer");

    userInputForm.addEventListener("submit", async function (event) {
        event.preventDefault();

        if (mainPage.style.display === "none") {
            titlePage.style.display = "none";
            mainPage.style.display = "flex";
            footer.style.display = "block";
            footer.style.position = "fixed";
        }

        const userInput = document.getElementById("user-input").value;

        if (!userInput.trim()) {
            return;
        }

        try {
            let maxNumResults = document.getElementById("max-num-results");
            let maxNumResultsValue = maxNumResults.value;
            if (maxNumResultsValue.length === 0) {
                maxNumResultsValue = maxNumResults.placeholder;
            } else if (!Number.isFinite(Number(maxNumResultsValue))) {
                maxNumResults.value = "";
                return;
            } else {
                maxNumResultsValue = Number(maxNumResults.value);
            }
            
            const response = await fetch("/submit", {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded",
                    "Max-Num-Responses": maxNumResultsValue
                },
                body: `user_input=${encodeURIComponent(userInput)}`
            });
            
            if (response.ok) {
                const data = await response.text();
                const parsedData = JSON.parse(data);
                const id = Date.now();

                queryStorage.saveQuery(userInput, parsedData, id);
                queryStorage.renderHistory(userInput, id);
                displayResults(parsedData, userInput);
            } else {
                responseDiv.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            responseDiv.textContent = `Error: ${error.message}`;
            console.error("Error:", error);
        }

        document.querySelectorAll(".user-input").forEach((user_input) => {user_input.value = ""});
    });
});

function clearHistory() {
    queryStorage.clearHistory();
    webpageContentStorage.clearWebContent();
}

function onCloseMenuEnd() {
    setTimeout(() => {
        document.getElementById("expand-button").style.visibility = "visible";
        document.getElementById("expand-button").style.opacity = 1;
        document.getElementById("menu").removeEventListener('transitionend', onCloseMenuEnd);
    }, 50);
}

function expandMenu() {
    document.getElementById("expand-button").style.visibility = "hidden";
    document.getElementById("expand-button").style.opacity = 0;
    document.getElementById("close-button").style.display = "flex";
    document.getElementById("footer").style.left = "175px";
    document.getElementById("menu").classList.add('open');
}

function closeMenu() {
    document.getElementById("footer").style.left = "0px";
    document.getElementById("menu").classList.remove('open');
    document.getElementById("menu").addEventListener('transitionend', onCloseMenuEnd);
}