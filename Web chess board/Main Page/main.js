function addAffect (className, affect) {
    let elements = document.getElementsByClassName(className);
    if (elements == null) return;
    for (let i = 0; i < elements.length; i++) {
        elements[i].addEventListener('mouseover', function() {
            elements[i].classList.add(affect);
        });
        elements[i].addEventListener('mouseout', function() {
            elements[i].classList.remove(affect);
        })
    }
}


addAffect('btn', 'hover');
addAffect('navicon', 'hover');
addAffect('profileicon', 'hover');
addAffect('loginbtn', 'hover');

document.getElementsByClassName('profileicon')[0].addEventListener('click', function() {
    window.location.href = "login.html";
}); 

document.getElementsByClassName('logo')[0].addEventListener('click', function() {
    window.location.href = "index.html";
});

let btns = document.getElementsByClassName('btn');
if (btns.length > 0) {
    btns[0].addEventListener('click', function() {
    });

    btns[1].addEventListener('click', async function() {
        const response = await FetchAPI('http://localhost:5084/api/player/joinlobby', {
            method: "POST"
        });

        if (response && response.ok) {
            const data = await response.json();
            localStorage.setItem('ticket', data.Ticket || data.ticket);
            // window.location.href = "C:\\Users\\abdal\\Desktop\\Grad Project\\Web chess board\\board.html";
            window.location.href = 'http://localhost:3999/board.html';
        }
        else if (response == null) {
            window.location.href = "login.html";
        }
        else {
            console.error('Game server connection error');
            return;
        }
    });

    btns[2].addEventListener('click', function() {

    });

    btns[3].addEventListener('click', function() {

    });
}

function triggerError (string, icon) {
    let sign = document.getElementById('errormsg');
    let errorMsgCont = sign.querySelector('#msg');
    errorMsgCont.innerHTML = string;
    if (icon === "cross") {
        document.getElementById('tick').style.display = "none";
    }
    if (icon === "tick") {
        document.getElementById('cross').style.display = "none";
    }
    sign.style.display = "grid";
    setInterval(() => {
        sign.style.display = "none";
    }, 3000);
}

const loginbtn = document.getElementById('loginbtn');
if (loginbtn != null) {
    loginbtn.addEventListener('click', async function() {
        const username = document.getElementById('unlogin').value;
        const password = document.getElementById('passlogin').value;

        if (username === "" || password === "") {
            triggerError("Please fill the blanks", 'cross');
            return;
        }

        const creds = {
            username: username,
            password: password
        }
        const response = await fetch ('http://localhost:5084/api/auth/login', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(creds)
        });

        if (response.ok) {
            const data = await response.json();
            localStorage.setItem('AccessToken', (data.AccessToken || data.accessToken));
            localStorage.setItem('RefresherToken', (data.RefresherToken || data.accessToken));
            triggerError("Successful Login!", "tick");

            setInterval(() => {
                window.location.href = "index.html";
            }, 3000);
            
        }
        else {
            triggerError("Wrong Credentials!", 'cross');
        }

    });
}

const signupbtn = document.getElementById('signupbtn');


if (signupbtn != null) {
    signupbtn.addEventListener('click', async function () {
        const username = document.getElementsByClassName('signuptxtbox')[0].value;
        const email = document.getElementsByClassName('signuptxtbox')[1].value;
        const password = document.getElementsByClassName('signuptxtbox')[2].value;
        const confpass = document.getElementsByClassName('signuptxtbox')[3].value;

        if (username === "" || email === "" || password === "" || confpass === "") {
            triggerError('Please fill the blanks!', 'cross');
            return;
        }
        if ((password !== confpass)) {
            triggerError('Passwords do not match!', 'cross');
            return;
        }

        const creds = {
            username: username,
            password: password,
            email: email
        };
        const response = await fetch ('http://localhost:5084/api/auth/register', {
            method: 'POST', 
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(creds)
        });

        if (response.ok) {
            triggerError("Successful Signup!", "tick");
            setInterval(() => {
                window.location.href = "login.html";
            }, 3000);
        }
        else {
            triggerError("Username Already Taken!", "cross");
        }
    });
}

let logoutbtn = document.getElementById('logoutbtn');
logoutbtn.addEventListener('click', async function() {
    const response = await FetchAPI('http://localhost:5084/api/auth/logout', {
        method: 'POST'
    });
    
    if (response && response.ok) {
        localStorage.clear();
        triggerError("Successful Logout!", "tick");
    }
    setInterval(() => {
        window.location.href = "login.html";
    }, 3000);
})

async function FetchAPI (link, options = {}) {
    let accessToken = localStorage.getItem('AccessToken');
    let refresherToken = localStorage.getItem('RefresherToken');

    options.headers = {
        ...options.headers,
        'Authorization': `Bearer ${accessToken}`,
    }

    let response = await fetch (link, options);


    if (response.status === 401) {
        if (refresherToken === null) {
            localStorage.clear();
            return null;
        }
        else {
            const response2 = await fetch ('http://localhost:5084/api/auth/refresh', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ RefresherToken: refresherToken})
            });

            if (response2.ok) {
                const data = await response2.json();
                localStorage.setItem('RefresherToken', data.RefresherToken);
                localStorage.setItem('AccessToken', data.AccessToken);
                return FetchAPI(link, options);
            }
            else {
                localStorage.clear();
                return null;
            }
        }
    }
    else return response;
}