function init(self)
	require 'gui'
	local btn = g_object(require 'gui.widgets.button')

	self.v_widget = btn

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