function init(self)
	require 'gui'
	-- package.loaded['gui.widgets.box'] = nil
	local box = g_object(require 'gui.widgets.box')

	box.o_width  = 300
	box.o_height = 170
	box.o_color  = { 255, 255, 255 }

	self.v_widget = box	

	local w, h = self.v_widget.o_width, self.v_widget.o_height
	self.v_node:set_size(w, h)
end

function update(self)
	self.v_widget:update()
end

function draw(self)
	local x, y = self.v_node:get_pos()
	self.v_widget:draw(x, y)
end