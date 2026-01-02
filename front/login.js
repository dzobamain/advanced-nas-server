document.querySelector(".login-form").addEventListener("submit", e => {
    e.preventDefault();

    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;

    fetch("/login", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
    })
    .then(res => res.text())
    .then(text => {
        if (text === "LOGIN_OK") {
            window.location.href = "/front/storage.html";
        } 
        else if (text === "LOGIN_FAILED") {
            alert("Invalid credentials");
        }
    });
});
