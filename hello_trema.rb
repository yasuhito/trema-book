# encoding: utf-8

# Hello World!
class HelloTrema < Controller
  def switch_ready(datapath_id)
    info format('Hello %#x!', datapath_id)
  end
end
