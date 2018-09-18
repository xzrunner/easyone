-- create node Tex2DAsset

require "moon.sg"

local node = moon.sg.new_node("sg_tex2d_asset")
if node then
	node:set_pos(moon.mouse.get_position())
end