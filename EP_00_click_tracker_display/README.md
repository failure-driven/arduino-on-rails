# Episode 00 - click tracker display

1. Next we will want to increment the counter for any page request using Rack Middleware ...

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
