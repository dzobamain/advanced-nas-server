document.querySelector(".login-form").addEventListener("submit", (e) => {
    e.preventDefault();

    const username = document.getElementById("username").value.trim();
    const password = document.getElementById("password").value;

    fetch("/login", {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: `username=${encodeURIComponent(username)}&password=${encodeURIComponent(password)}`
    })
    .then((res) => {
        // If your server returns 302 on success, many browsers will follow it,
        // but fetch still won't "navigate". We navigate manually.
        if (res.status === 302 || res.redirected) {
            window.location.href = "/storage";
            return null;
        }

        // If your server returns 200/401 with text (old behavior)
        return res.text().then((text) => ({ status: res.status, text }));
    })
    .then((data) => {
        if (!data) return;

        // Old behavior support:
        if (data.text === "LOGIN_OK" || data.status === 200) {
            window.location.href = "/storage";
        } else {
            alert("Invalid credentials");
        }
    })
    .catch(() => {
        alert("Server error");
    });
});
