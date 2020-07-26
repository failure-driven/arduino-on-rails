class ClickCounter
  def initialize(app)
    @app = app
  end

  def call(env)
    @status, @headers, @response = @app.call(env)
    @count = Counter.increment_default_counter
    [@status, @headers, self]
  end

  def each(&block)
    block.call("<!-- Counter: #{@count} -->\n") if @headers['Content-Type'].include?('text/html')
    @response.each(&block)
  end
end
