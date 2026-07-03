class CallHelper:

    def __init__(self, settings: dict):

        self.settings = settings

    def login(self):
        print("Called login")
        return True

    def logout(self):
        print("Called logout")
        return True

    def checkup(self):
        if 'first' in self.settings:
            pass
        else:
            print("Called checkout")
            self.settings['first'] = True
        return True
