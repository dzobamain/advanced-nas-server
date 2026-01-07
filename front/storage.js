const foldersList = [
    "Documents",
    "Photos",
    "Videos",
    "Backups",
    "Projects",
    "Music",
    "Archives"
];

const container = document.getElementById("folders");

function renderFolders(folders) {
    container.innerHTML = "";

    folders.forEach(name => {
        const folder = document.createElement("div");
        folder.className = "folder";

        folder.innerHTML = `
            <div class="folder-icon"></div>
            <div class="folder-name">${name}</div>
        `;

        folder.onclick = () => {
            console.log("Open folder:", name);
        };

        container.appendChild(folder);
    });
}

renderFolders(foldersList);
