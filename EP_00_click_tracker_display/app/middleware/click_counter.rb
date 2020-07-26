class ClickCounter
  def initialize(app)
    @app = app
  end

  def call(env)
    status, headers, response = @app.call(env)
    Counter.increment_default_counter
    [status, headers, response]
  end
end
