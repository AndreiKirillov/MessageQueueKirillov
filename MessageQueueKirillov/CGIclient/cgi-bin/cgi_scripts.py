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

def messageWasSendSuccessfully():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Success</title>
            </head>
            <body>
                <h1>Message was send successfully!</h1>
                <form action="../main_page.html">
                <button>Continue work</button>
                </form>
            </body>
            </html>""")

def messageError():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Error</title>
            </head>
            <body>
                <h1>Server Error! Message was not sent!</h1>
                <form action="../main_page.html">
                <button>Continue work</button>
                </form>
            </body>
            </html>""")

def exitFromServer():
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Exit</title>
            </head>
            <body>
                <h1>You exited from server!</h1>
            </body>
            </html>""")

def showAllMessages(messages_list):
    print("Content-type: text/html; charset=utf-8\n\n")
    print("""<!DOCTYPE HTML>
            <html>
            <head>
                <meta charset="utf-8">
                <title>Messages</title>
            </head>
            <body>""")
    for message in messages_list:
        print(f"<h3>{message}</h3")
    print("""<br>
            <form action="../main_page.html">
                <button>Continue work</button>
                </form>
            </body>
            </html>""")