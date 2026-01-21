document.addEventListener("DOMContentLoaded", function() {
    const mainPage = document.getElementById("main-page");
    const footer = document.getElementById("footer");
    mainPage.style.display = "none";
    footer.style.display = "none";
});

document.addEventListener("DOMContentLoaded", function() {
    const userInputForms = document.querySelectorAll(".user-input-form");
    const responseDiv = document.getElementById("response");
    const mainPage = document.getElementById("main-page");
    const titlePage = document.getElementById("title-page");
    const footer = document.getElementById("footer");

    const exampleResults = "{\"results\":[{\"title\":\"Question about realloc : r\/cprogramming\",\"link\":\"https:\/\/www.reddit.com\/r\/cprogramming\/comments\/1lrkzjb\/question_about_realloc\/\", \"id\":\"result_1\"},{\"title\":\"Proper usage of realloc()\",\"link\":\"https:\/\/stackoverflow.com/questions\/21006707\/proper-usage-of-realloc\", \"id\":\"result_2\"},{\"title\":\"OpenGL - Picking (fastest way)\",\"link\":\"https:\/\/stackoverflow.com\/questions\/28032910\/opengl-picking-fastest-way\", \"id\":\"result_3\"},{\"title\":\"How to handle realloc when it fails due to memory?\",\"link\":\"https:\/\/stackoverflow.com\/questions\/1986538\/how-to-handle-realloc-when-it-fails-due-to-memory\"},{\"title\":\"How do birds fly? We explain how they get - and stay - airborne ...\",\"link\":\"https:\/\/www.discoverwildlife.com\/animal-facts\/birds\/how-do-birds-fly\", \"id\":\"result_5\"}]}"; 
    console.log("ASDSADAS");

    userInputForms.forEach((userInputForm) => {
        console.log("ASDSADAS");
        userInputForm.addEventListener("submit", async function (event) {
            event.preventDefault();
            console.log("ASDSADAS");

            if (mainPage.style.display === "none") {
                console.log("CHANGING");
                titlePage.style.display = "none";
                mainPage.style.display = "block";
                footer.style.display = "block";
            }
            displayResults(JSON.parse(exampleResults), "Example query");
            return;

            // const userInput = document.getElementById("user-input").value;

            // if (!userInput.trim()) {
            //     return;
            // }

            // try {
            //     const response = await fetch("/submit", {
            //         method: "POST",
            //         headers: {
            //             "Content-Type": "application/x-www-form-urlencoded",
            //         },
            //         body: `user_input=${encodeURIComponent(userInput)}`
            //     });
                
            //     if (response.ok) {
            //         const data = await response.text();
            //         displayResults(JSON.parse(data), userInput);
            //     } else {
            //         responseDiv.textContent = `Error: ${response.status}`;
            //     }
            // } catch (error) {
            //     responseDiv.textContent = `Error: ${error.message}`;
            //     console.error("Error:", error);
            // }

            // document.querySelectorAll(".user-input").forEach((user_input) => {user_input.value = ""});
        });
    });

    function displayResults(data, query) {
        responseDiv.innerHTML = "";

        const queryElement = document.createElement("div");
        queryElement.className = "response-header";
        queryElement.innerHTML = `<h2>Results for: "${query}"</h2>`;
        responseDiv.appendChild(queryElement);
        
        if (!data.results || data.results.length === 0) {
            responseDiv.innerHTML += `<div class="no-results">No results!</div>`;
            return;
        }
        
        const responseContent = document.createElement("div");
        responseContent.className = "response-content";

        data.results.forEach((result, index) => {
            const resultElement = document.createElement("div");
            resultElement.className = "result-item";
            resultElement.id = `content_${result.id}`;
            resultElement.innerHTML = `
                <div class="result-header">
                    <div class="result-expander">
                    </div>
                    <a href="${result.link}">
                        <h3 class="result-title">${result.title}</h3>
                    </a>
                </div>
                <div class="result-content">
                </div>
            `;
            const resultExpander = resultElement.querySelector(".result-expander");
            const resultContent = resultElement.querySelector(".result-content");
            resultExpander.textContent = "▼";
            resultContent.style.display = "none";
            resultExpander.addEventListener("click", async function() {
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
            responseContent.appendChild(resultElement);
        });
        
        responseDiv.appendChild(responseContent);
    }

    async function fetchAndDisplayContent(resultId, contentArea) {
        try {
            const response = await fetch(`/content?url=${encodeURIComponent(resultId)}`);
            
            if (response.ok) {
                const escaped = response.headers.get("Html-Escaped") === "true";
                const data = await response.text();
                const jsonData = JSON.parse(data);
                contentArea.innerHTML += `<div class="content-display">`;
                jsonData.content.forEach((item) => {
                    let html = item.content_body;

                    if (escaped) {
                        const txt = document.createElement("textarea");
                        txt.innerHTML = html;
                        html = txt.value;
                    }
                    contentArea.innerHTML += `
                        <div class="content-text">
                            ${html}
                        </div>
                    `;
                });
                contentArea.innerHTML += `</div>`;
                contentArea.dataset.loaded = true;
            } else {
                contentArea.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            contentArea.textContent = `Error: ${error.message}`;
            console.error("Error:", error);
        }
    }
});