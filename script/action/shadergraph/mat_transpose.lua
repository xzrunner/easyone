-- create node MatrixTranspose

require "moon.sg"

local node = moon.sg.new_node("sg_mat_transpose")
if node then
	node:set_pos(moon.mouse.get_position())
end