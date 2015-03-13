require 'routing_switch/api'

describe RoutingSwitch::Api do
  include Rack::Test::Methods

  def app
    RoutingSwitch::Api
  end

  describe 'GET /api/slices' do
    it 'lists slices' do
      get '/api/slices'
      expect(last_response.status).to eq(200)
      expect(JSON.parse(last_response.body)).to eq([])
    end
  end
end
