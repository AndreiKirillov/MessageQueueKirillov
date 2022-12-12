
def afterSuccessfullRegistration():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Log in</title>
            </head>
            <body>
                <h1>You logged in successfully!</h1>
                <form action="../main_page.html">
                <button>Begin work</button>
                </form>
            </body>
            </html>""")

def registrationFailed():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Login error</title>
            </head>
            <body>""")
    print("<h1>You cannot log in the server!</h1>")
    print("""</body>
        </html>""")

def emptyLoginForm():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Login warning</title>
            </head>
            <body>""")
    print("<h1>You didnt input your name!</h1>")
    print("""</body>
        </html>""")