option("env")
    set_showmenu(true)
    set_default(true)

    local project_dir = "$(projectdir)"
    local shader_dir = "$(projectdir)\\shaders"

    set_configvar("PROJECT_DIR", project_dir, {escape = true})
    set_configvar("SHADER_DIR", shader_dir, {escape = true})
