import os

def print_tree(startpath, prefix=""):
    items = os.listdir(startpath)
    for index, item in enumerate(items):
        path = os.path.join(startpath, item)
        if index == len(items) - 1:
            connector = "└── "
            new_prefix = prefix + "    "
        else:
            connector = "├── "
            new_prefix = prefix + "│   "
        
        print(prefix + connector + item)
        if os.path.isdir(path):
            print_tree(path, new_prefix)

if __name__ == "__main__":
    print("Directory Tree of the Current Directory:")
    print_tree(os.getcwd())
