import config, csv, os

# gl = gitlab.Gitlab('https://gitlab.com', private_token=config.gitlab_token)


def write_recoll_config():
    if not os.path.exists(os.path.join(config.root_dir, ".recoll")):
        os.mkdir(os.path.join(config.root_dir, ".recoll"))
    if not os.path.exists(os.path.join(config.root_dir, ".recoll", "recoll.conf")):
        with open(os.path.join(config.root_dir, ".recoll", "recoll.conf"), "w") as recoll_config_file:
            print(f"topdirs = {os.path.join(config.root_dir, 'checkout')}", file=recoll_config_file)

with open(os.path.join(config.root_dir, "project_list.csv")) as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        print(row[0])
        project = row[0].split('/')[-1]
        os.chdir(os.path.join(config.root_dir, "checkout"))
        if not os.path.exists(os.path.join(config.root_dir, "checkout", project)):
            os.system(f"git clone https://{config.github_username}:{config.github_token}@{row[0][8:]}")

    write_recoll_config()
    os.chdir(config.root_dir)
    os.system("recollindex -c .recoll")



# list all the projects
# projects = gl.projects.list()
#for project in projects:
#    print(project)