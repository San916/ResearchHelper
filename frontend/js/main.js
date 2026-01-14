document.addEventListener('DOMContentLoaded', function() {
    const user_input_form = document.getElementById('user_input_form');
    const responseDiv = document.getElementById('response');

    // const example_results = "{\"results\":[{\"title\":\"How does a bird fly? : r\/askscience\",\"link\":\"https:\/\/www.reddit.com\/r\/askscience\/comments\/1hu74a8\/how_does_a_bird_fly\/\"},{\"title\":\"How birds fly \u2014 Science Learning Hub\",\"link\":\"https:\/\/www.sciencelearn.org.nz\/resources\/303-how-birds-fly\"},{\"title\":\"The Physics of Flight - Schlitz Audubon\",\"link\":\"https:\/\/www.schlitzaudubon.org\/2022\/09\/19\/the-physics-of-flight\/\"},{\"title\":\"Do birds fly around just for \\\"fun\\\"? : r\/ecology\",\"link\":\"https:\/\/www.reddit.com\/r\/ecology\/comments\/px70ok\/do_birds_fly_around_just_for_fun\/\"},{\"title\":\"How do birds fly? We explain how they get - and stay - airborne ...\",\"link\":\"https:\/\/www.discoverwildlife.com\/animal-facts\/birds\/how-do-birds-fly\"}]}"; 

    user_input_form.addEventListener('submit', async function (event) {
        event.preventDefault();

        // display_results(JSON.parse(example_results), "Example query");
        // return;

        const user_input = document.getElementById('user_input').value;

        if (!user_input.trim()) {
            return;
        }

        try {
            const response = await fetch('/submit', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: `user_input=${encodeURIComponent(user_input)}`
            });
            
            if (response.ok) {
                const data = await response.text();
                display_results(JSON.parse(data), user_input);
            } else {
                responseDiv.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            responseDiv.textContent = `Error: ${error.message}`;
            console.error('Error:', error);
        }

        document.getElementById('user_input').value = '';
    });

    function display_results(data, query) {
        responseDiv.innerHTML = '';

        const queryElement = document.createElement('div');
        queryElement.className = 'response_header';
        queryElement.innerHTML = `<h2>Results for: "${query}"</h2>`;
        responseDiv.appendChild(queryElement);
        
        if (!data.results || data.results.length === 0) {
            responseDiv.innerHTML += '<div class="no_results">No results!</div>';
            return;
        }
        
        const resultsContainer = document.createElement('div');
        resultsContainer.className = 'results_container';

        data.results.forEach((result, index) => {
            const resultElement = document.createElement('div');
            resultElement.className = 'result_item';
            resultElement.innerHTML = `
                <div class="result_expander">
                </div>
                <div class="result_content">
                    <a href="${result.link}">
                        <h3 class="result_title">${result.title}</h3>
                    </a>
                </div>
            `;
            resultsContainer.appendChild(resultElement);
        });
        
        responseDiv.appendChild(resultsContainer);
    }
});