document.addEventListener('DOMContentLoaded', function() {
    const user_input_form = document.getElementById('user_input_form');
    const responseDiv = document.getElementById('response');

    user_input_form.addEventListener('submit', async function (event) {
        event.preventDefault();

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
                responseDiv.textContent = `Server response: ${data}`;
            } else {
                responseDiv.textContent = `Error: ${response.status}`;
            }
        } catch (error) {
            responseDiv.textContent = `Error: ${error.message}`;
            console.error('Error:', error);
        }
        
        // Clear the input field
        document.getElementById('user_input').value = '';
    });
});