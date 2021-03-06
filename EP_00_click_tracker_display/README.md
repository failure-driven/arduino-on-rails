# Episode 00 - click tracker display

1. finally try to get the count using Arduino, or ESP32 with WiFi

  - connect to WiFi

  ```c
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Waiting for WiFi... ");

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    Serial.print(".");
    delay(500);
  }
  ```

  - fetch https request

  ```c
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(DIGICERT_CA_CERTIFICATE);
    if ((WiFi.status() == WL_CONNECTED)) {
      HTTPClient https;
      https.begin(*client, URL);
      int httpsCode = https.GET();
      if (httpsCode > 0) {
        String payload = https.getString();
      }
    }
  }
  ```

  - parse out the JSON

  ```c
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  if(error) {
    Serial.print("deserializeJson() failed: ");
    Serial.print(error.c_str());
    Serial.println(payload);
    return;
  }
  long countStat = doc[0]["count"];
  ```

  - write out to 7seg display

  ```c
  showNumber(countStat);
  ```

1. for fun let's also add an **`X-Request-Count`** header. This is not a standard header in any way but will come back as part of every request

   `app/middleware/click_counter.rb`

   ```ruby
   def call(env)
      @status, @headers, @response = @app.call(env)
      @count = Counter.increment_default_counter
      @headers.merge!('X-Request-Count' => @count.to_s)
      [@status, @headers, self]
   end
   ```

   viewing headers using `curl` and grepping for headers starting in `X-R`

   ```sh
   curl -I http://localhost:3000 | grep X-R

   X-Request-Count: 20
   X-Request-Id: 51d3035f-7c8d-4ea2-a3ed-01d857fa05e1
   X-Runtime: 0.022604
   ```

1. let's add some default pages

   we could just make the welcome rails page work in production by explicitly adding it to the routes file as the root

   ```ruby
   Rails.application.routes.draw do
      ...
      root to: "rails/welcome#index"
   end
   ```

   Another deploy which needs a tweak to the deploy script

   `bin/heroku_deploy`

   ```ruby
   # first time deploy
   git subtree push --prefix EP_00_click_tracker_display/ heroku master

   # subsequent time
   git push --force heroku `git subtree split --prefix EP_00_click_tracker_display master`:master
   ```

   **WHY???**

   ```sh
   heroku open

   # also
   heroku run rails runner 'puts Counter.all.map(&:attributes)'

   # and hit the counter some
   heroku run rails runner 'exit_requested = false; Kernel.trap( "INT" ) { exit_requested = true }; while !exit_requested; Counter.increment_default_counter; print "."; sleep(0.2); end'

   # ofcouse so does curl
   curl https://ep-0-click-tracker-display.herokuapp.com/counters
   ```

1. now to deploy to heroku using a simple script `bin/heroku_deploy`

   ```sh
   HEROKU_APP_NAME=ep-0-click-tracker-display bin/heroku_deploy
   ```

   finally visit the page

   ```sh
   heroku open
   ```

   :( there is no page

   https://ep-0-click-tracker-display.herokuapp.com/

   but there are counters

   https://ep-0-click-tracker-display.herokuapp.com/counters

1. for good measure let's have the middleware update a header with the current count

   `app/middleware/click_counter.rb`

   ```ruby
   def call(env)
      @status, @headers, @response = @app.call(env)
      @count = Counter.increment_default_counter
      [@status, @headers, self]
   end

   def each(&block)
      block.call("<!-- Counter: #{@count} -->\n") if @headers['Content-Type'].include?('text/html')
      @response.each(&block)
   end
   ```

   and add a to string method for a counter

   `app/models/counter.rb`

   ```ruby
   class Counter < ApplicationRecord
      ...
      def to_s
         count.to_s
      end
   end
   ```

   now at the top of every html page we get a comment with the counter

   view-source:http://localhost:3000/

1. Let's add some Rack Middleware to increment the counter on every page visit

   ```sh
   mkdir app/middleware

   cat > app/middleware/click_counter.rb
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
   ^D
   ```

   and add the middleware to the end of the middleware stack

   `config/application.rb`

   ```ruby
   $LOAD_PATH << File.join(__dir__, '..', 'app', 'middleware')
   require 'click_counter'

   ...
   class Application < Rails::Application
      ...
      config.middleware.use ClickCounter
   end
   ```

   Now any visit to the site will increment the counter

   - http://localhost:3000
   - http://localhost:3000/counter

   and view the counter without calling the page

   ```sh
   bin/rails runner 'puts Counter.all.map(&:attributes)'
   ```

1. but the `CTRL-C` blows up with an interrupt and a huge stack trace, I know why not catch the interrupt and just gracefully catch the error

   First tried wrapping it in excpetion handling

   ```sh
   bin/rails runner 'begin; while true; Counter.increment_default_counter; print "."; sleep(0.2); end; resuce Interrupt => e; puts "END, ${e}"; end;'
   ```

   didn't seem to work :(

   searching for _"rails runner interrupt infinite loop"_

   found this https://stackoverflow.com/questions/4508764/how-can-i-add-a-user-interrupt-to-an-infinite-loop

   which did work

   ```sh
   bin/rails runner 'exit_requested = false; Kernel.trap( "INT" ) { exit_requested = true }; while !exit_requested; Counter.increment_default_counter; print "."; sleep(0.2); end'
   ```

1. let's add a `Counter.increment_default_counter` function to incremenet our default Counter

   ```ruby
   class Counter < ApplicationRecord
      def self.increment_default_counter
         Counter
            .find_or_create_by(name: 'default')
            .increment!(:count)
      end
   end
   ```

   which can be called with

   ```sh
   bin/rails runner 'Counter.increment_default_counter'
   ```

   and made to run indefinitely every 200ms with

   ```sh
   bin/rails runner 'while true; Counter.increment_default_counter; print "."; sleep(0.2); end'
   ```

   and `CTRL-C` to kill it

1. And a controller to view the count

   ```sh
   rails generate controller Counters index  \
      --template-engine=jbuilder             \
      --no-javascripts                       \
      --no-stylesheets                       \
      --no-helper
   ```

   and modify the `config/routes.rb` to allow index counters but only in `json` format

   ```ruby
   Rails.application.routes.draw do
     resources :counters, only: %i[index], defaults: { format: :json }
   end
   ```

   finally `find_or_create` our page counter

   ```ruby
   class CountersController < ApplicationController
      def index
         @counters = Counter.all
      end
   end
   ```

   and display the count in the view which is modified to a jbuilder template

   `app/views/counters/index.html.jbuilder`

   ```jbuilder
   json.array! @counters, :id, :name, :count
   ```

   viewing in the borwser shows no counters

   - http://localhost:3000/counters
   - http://localhost:3000/counters.json

   now from the command line create our default counter

   ```sh
   bin/rails runner 'Counter.find_or_create_by(name: "default")'
   ```

   and now a count of **0** is displayed

   http://localhost:3000/counters.json

   ```json
   {
     "counters": [
       {
         "id": 1,
         "name": "default",
         "count": 0
       }
     ]
   }
   ```

1. Now lets add a click tracker, first a counter model

   ```sh
   rails generate model Counter name:string count:bigint
   bin/rails db:create db:migrate
   ```

   alter this to make count default to 0, name to be unique and both count and nam e to be mandatory

   ```ruby
      class CreateCounters < ActiveRecord::Migration[6.0]
         def change
            create_table :counters do |t|
               t.string :name, null: false, index: { unique: true }
               t.bigint :count, null: false, default: 0

               t.timestamps
            end
         end
      end
   ```

1. You should see the familiar **"Yay! You’re on Rails!"**

1. Fire up rails and view it in the browser

   ```
   bin/rails server
   open http://localhost:3000
   ```

1. create a minimal `rails new`
   ```
   rails new EP_00_click_tracker_display \
     --database=postgresql               \
     --skip-action-mailer                \
     --skip-action-mailbox               \
     --skip-action-text                  \
     --skip-active-storage               \
     --skip-puma                         \
     --skip-action-cable                 \
     --skip-sprockets                    \
     --skip-spring                       \
     --skip-javascript                   \
     --skip-turbolinks                   \
     --skip-test                         \
     --skip-system-test                  \
     --skip-webpack-install              \
     --skip-keeps
   ```

# ORIGINAL README

This README would normally document whatever steps are necessary to get the
application up and running.

Things you may want to cover:

- Ruby version

- System dependencies

- Configuration

- Database creation

- Database initialization

- How to run the test suite

- Services (job queues, cache servers, search engines, etc.)

- Deployment instructions

- ...
