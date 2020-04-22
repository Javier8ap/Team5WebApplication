"use strict";

let userEmail = document.querySelector('#userEmail');
let userPassword = document.querySelector('#userPassword');

function logIn(){
    
    let xhr = new XMLHttpRequest();
    let endpoint = `http://localhost:3000/users/?Email=${userEmail.value}`
    xhr.open('GET', endpoint);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send();
    xhr.onload = () => {
        if (xhr.status == 200) {
            console.log("Entra2")
            let user = JSON.parse(xhr.response);
            if (user[0].Password == userPassword.value) {
                window.location.href="menu.html";
            }else{
                alert("Invalid Password");
            }
        } else if (xhr.status == 404) {
            alert("Invalid Username");
        }
    }
    
}


