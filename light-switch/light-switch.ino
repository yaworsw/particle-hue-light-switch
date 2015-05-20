#define SLIDER1  A0
#define SLIDER2  A1

#define LIGHT1   "1"
#define LIGHT2   "2"

#define HUE_HUB_IP_ADDRESS_BYTES {192,168,1,92}
#define HUE_HUB_IP_ADDRESS "192.168.1.92"
#define HUE_DEVELOPER_NAME "newdeveloper"

#define GRANULARITY      200
#define MAX_SLIDER_VALUE 4017

int slider1;
int slider2;

int last_slider1 = 0;
int last_slider2 = 0;

void setup(void)
{
  Serial.begin(9600);

  pinMode(SLIDER1, INPUT);
  Spark.variable("slider1", &slider1, INT);
  pinMode(SLIDER2, INPUT);
  Spark.variable("slider2", &slider2, INT);
}

void loop()
{
  slider1 = analogRead(SLIDER1);
  slider2 = analogRead(SLIDER2);

  if (update_lights(LIGHT1, slider1, last_slider1))
  {
    last_slider1 = slider1;
  }

  if (update_lights(LIGHT2, slider2, last_slider2))
  {
    last_slider2 = slider2;
  }
}

bool update_lights(String light_id, int slider_val, int last_slider_val)
{
  if (abs(slider_val - last_slider_val) > GRANULARITY)
  {
    TCPClient client;
    client.connect(HUE_HUB_IP_ADDRESS_BYTES, 80);

    if (client.connected()) {

      char* content = light_json(slider_val);

      char* req;
      asprintf(&req, "PUT /api/%s/lights/%s/state HTTP/1.0", HUE_DEVELOPER_NAME, light_id.c_str());

      char* host;
      asprintf(&host, "Host: %s", HUE_HUB_IP_ADDRESS);

      char* content_length;
      asprintf(&content_length, "Content-Length: %i", strlen(content));

      client.println(req);
      client.println(host);
      client.println("Content-Type: application/json");
      client.println(content_length);
      client.println();
      client.println(content);

      Serial.println(req);
      Serial.println(host);
      Serial.println("Content-Type: application/json");
      Serial.println(content_length);
      Serial.println();
      Serial.println(content);

      client.flush();
      client.stop();
    }
    else
    {
      Serial.println("Connection failed");
      return false;
    }

    return true;
  }
  else
  {
    return false;
  }
}

char* light_json(int slider_val)
{
  if (slider_val < 25)
  {
    return "{\"on\":false}";
  }
  else
  {
    Serial.println();
    Serial.println(slider_val);
    Serial.println();
    int bri = int((float(slider_val) / MAX_SLIDER_VALUE) * 255);
    if (bri > 255)
    {
      bri = 255;
    }
    char* res;
    asprintf(&res, "{\"on\":true,\"bri\":%i}", bri);
    return res;
  }
}
