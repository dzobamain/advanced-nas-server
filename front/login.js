document.querySelector(".login-form").addEventListener("submit", e =>{
    e.preventDefault();

    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;

    fetch("/login", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: `username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
    })
    .then(res => {
        if (res.status === 200) {
            window.location.href = "/front/data.html";
        } else {
            alert("Invalid credentials");
        }
    });
});
