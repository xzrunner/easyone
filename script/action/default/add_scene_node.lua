local filename = moon.gui.open_file_dlg("select scene node")
moon.scene.new_node(filename, moon.mouse.get_position())