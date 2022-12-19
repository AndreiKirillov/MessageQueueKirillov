import global_vars
def update_user(new_name):
    global_vars.username = new_name

def get_user():
    return global_vars.username
