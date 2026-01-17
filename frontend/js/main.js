document.addEventListener("DOMContentLoaded", function() {
    const userInputForm = document.getElementById("user-input-form");
    const responseDiv = document.getElementById("response");

    const exampleResults = "{\"results\":[{\"title\":\"How does a bird fly? : r\/askscience\",\"link\":\"https:\/\/www.reddit.com\/r\/askscience\/comments\/1hu74a8\/how_does_a_bird_fly\/\", \"id\":\"result_1\"},{\"title\":\"How birds fly \u2014 Science Learning Hub\",\"link\":\"https:\/\/www.sciencelearn.org.nz\/resources\/303-how-birds-fly\", \"id\":\"result_2\"},{\"title\":\"The Physics of Flight - Schlitz Audubon\",\"link\":\"https:\/\/www.schlitzaudubon.org\/2022\/09\/19\/the-physics-of-flight\/\", \"id\":\"result_3\"},{\"title\":\"Do birds fly around just for \\\"fun\\\"? : r\/ecology\",\"link\":\"https:\/\/www.reddit.com\/r\/ecology\/comments\/px70ok\/do_birds_fly_around_just_for_fun\/\", \"id\":\"result_4\"},{\"title\":\"How do birds fly? We explain how they get - and stay - airborne ...\",\"link\":\"https:\/\/www.discoverwildlife.com\/animal-facts\/birds\/how-do-birds-fly\", \"id\":\"result_5\"}]}"; 

    userInputForm.addEventListener("submit", async function (event) {
        event.preventDefault();

        displayResults(JSON.parse(exampleResults), "Example query");
        return;

        const userInput = document.getElementById("user-input").value;

        if (!userInput.trim()) {
            return;
        }

        try {
            const response = await fetch("/submit", {
                method: "POST",
                headers: {
                    "Content-Type": "application/x-www-form-urlencoded",
                },
                body: `user_input=${encodeURIComponent(userInput)}`
            });
            
            if (response.ok) {
                const data = await response.text();
                displayResults(JSON.parse(data), userInput);
            } else {
                responseDiv.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            responseDiv.textContent = `Error: ${error.message}`;
            console.error("Error:", error);
        }

        document.getElementById("user-input").value = "";
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
                const data = await response.json();
                contentArea.innerHTML = `
                    <div class="content-display">
                        <div class="content-text">${data.content}</div>
                    </div>
                `;
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