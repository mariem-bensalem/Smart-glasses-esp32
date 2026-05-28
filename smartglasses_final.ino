#include <WiFi.h>
#include <WebServer.h>

const char* ssid     = "Mariem";
const char* password = "12345678";

#define TRIG_PIN   14
#define ECHO_PIN   12
#define BUZZER_PIN 13

WebServer server(80);

unsigned long lastBuzzTime = 0;
bool buzzerState = false;

long getDistance() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 30000) * 0.034 / 2;
  if (d <= 0 || d > 400) return 999;
  return d;
}

void handleBuzzer(long dist) {
  unsigned long now = millis();
  if (dist == 999 || dist > 60) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
    return;
  }
  int onTime  = (dist <= 20) ? 80  : 200;
  int offTime = (dist <= 20) ? 80  : 600;
  if (buzzerState && now - lastBuzzTime >= (unsigned long)onTime) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = false;
    lastBuzzTime = now;
  } else if (!buzzerState && now - lastBuzzTime >= (unsigned long)offTime) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerState = true;
    lastBuzzTime = now;
  }
}


const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
<title>Smart Glasses</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Syne:wght@700;800&family=DM+Mono:wght@400;500&family=DM+Sans:wght@300;400;500&display=swap');
  :root {
    --danger: #e83a3a;
    --caution: #e87c3a;
    --safe: #3ae87c;
    --bg: #080c0e;
    --surface: #0f1518;
    --border: #1a2228;
    --text: #c8d8e0;
    --muted: #3a5060;
  }
  * { margin:0; padding:0; box-sizing:border-box; }
  body {
    background: var(--bg);
    color: var(--text);
    font-family: 'DM Sans', sans-serif;
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 40px 20px 32px;
  }
  /* subtle noise texture overlay */
  body::after {
    content:'';
    position:fixed; inset:0;
    background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 256 256' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='n'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.9' numOctaves='4' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23n)' opacity='0.04'/%3E%3C/svg%3E");
    pointer-events:none; z-index:0;
  }

  .wrap {
    position: relative; z-index: 1;
    width: 100%; max-width: 360px;
    display: flex; flex-direction: column; gap: 20px;
  }

  /* ── HEADER ── */
  header { display: flex; flex-direction: column; gap: 2px; }
  .brand {
    font-family: 'Syne', sans-serif;
    font-size: 22px; font-weight: 800;
    letter-spacing: -0.5px;
    color: #e8f0f4;
  }
  .brand span { color: var(--safe); }
  .tagline {
    font-family: 'DM Mono', monospace;
    font-size: 10px; letter-spacing: 2px;
    color: var(--muted); text-transform: uppercase;
  }

  /* ── RADAR RING ── */
  .radar-wrap {
    position: relative;
    width: 220px; height: 220px;
    margin: 8px auto 0;
  }
  .radar-ring {
    width: 220px; height: 220px;
    border-radius: 50%;
    background: var(--surface);
    border: 1.5px solid var(--border);
    display: flex; flex-direction: column;
    align-items: center; justify-content: center;
    position: relative; overflow: hidden;
    transition: border-color 0.5s, box-shadow 0.5s;
  }
  .radar-ring::before {
    content: '';
    position: absolute; inset: 0;
    border-radius: 50%;
    background: radial-gradient(circle at 50% 50%, transparent 40%, rgba(58,232,124,0.04) 100%);
    transition: background 0.5s;
  }
  /* concentric guide rings */
  .radar-ring::after {
    content: '';
    position: absolute;
    width: 140px; height: 140px;
    border-radius: 50%;
    border: 1px solid var(--border);
    top: 50%; left: 50%;
    transform: translate(-50%, -50%);
  }
  .ring-inner {
    position: absolute;
    width: 70px; height: 70px;
    border-radius: 50%;
    border: 1px solid var(--border);
    top: 50%; left: 50%;
    transform: translate(-50%, -50%);
  }
  .sweep-line {
    position: absolute; inset: 0;
    border-radius: 50%;
    background: conic-gradient(from 0deg, transparent 0%, rgba(58,232,124,0.15) 12%, transparent 12%);
    animation: sweep 2.5s linear infinite;
    transition: background 0.4s;
  }
  @keyframes sweep { to { transform: rotate(360deg); } }

  .dist-value {
    font-family: 'Syne', sans-serif;
    font-size: 58px; font-weight: 800;
    color: var(--safe);
    line-height: 1; letter-spacing: -2px;
    position: relative; z-index: 1;
    transition: color 0.3s;
  }
  .dist-unit {
    font-family: 'DM Mono', monospace;
    font-size: 11px; letter-spacing: 3px;
    color: var(--muted);
    position: relative; z-index: 1;
    margin-top: 2px;
  }

  /* ── STATUS LINE ── */
  .status-line {
    display: flex; align-items: center; gap: 8px;
  }
  .status-dot {
    width: 7px; height: 7px; border-radius: 50%;
    background: var(--safe);
    box-shadow: 0 0 8px var(--safe);
    transition: background 0.3s, box-shadow 0.3s;
    flex-shrink: 0;
  }
  .status-dot.pulse { animation: dotpulse 1s ease-in-out infinite; }
  @keyframes dotpulse {
    0%,100% { transform: scale(1); opacity: 1; }
    50% { transform: scale(1.6); opacity: 0.5; }
  }
  .status-text {
    font-family: 'DM Mono', monospace;
    font-size: 12px; letter-spacing: 1.5px;
    color: var(--safe);
    text-transform: uppercase;
    transition: color 0.3s;
  }

  /* ── ALERT BOX ── */
  .alert-box {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 14px 16px;
    font-size: 13px;
    color: var(--muted);
    min-height: 48px;
    display: flex; align-items: center;
    transition: border-color 0.3s, color 0.3s, background 0.3s;
  }

  /* ── BUTTON ── */
  .btn {
    width: 100%; padding: 16px;
    border: 1px solid var(--safe);
    border-radius: 10px;
    background: transparent;
    color: var(--safe);
    font-family: 'DM Mono', monospace;
    font-size: 12px; letter-spacing: 2px;
    text-transform: uppercase;
    cursor: pointer;
    transition: background 0.2s, color 0.2s, border-color 0.2s;
  }
  .btn:hover { background: rgba(58,232,124,0.06); }
  .btn:active { transform: scale(0.98); }
  .btn.on {
    border-color: var(--danger);
    color: var(--danger);
  }
  .btn.on:hover { background: rgba(232,58,58,0.06); }

  /* ── RANGE LEGEND ── */
  .legend {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
    gap: 8px;
  }
  .legend-item {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: 10px 6px;
    text-align: center;
  }
  .legend-range {
    font-family: 'DM Mono', monospace;
    font-size: 11px; font-weight: 500;
    letter-spacing: 0.5px;
  }

  /* ── FOOTER ── */
  .footer {
    font-family: 'DM Mono', monospace;
    font-size: 10px; letter-spacing: 1px;
    color: var(--muted);
    text-align: center;
  }
  .divider {
    height: 1px; background: var(--border);
  }
</style>
</head>
<body>
<div class="wrap">

  <header>
    <div class="brand">Smart <span>Glasses</span></div>
    <div class="tagline">Obstacle Detection System</div>
  </header>

  <div class="radar-wrap">
    <div class="radar-ring" id="radarRing">
      <div class="ring-inner"></div>
      <div class="sweep-line" id="sweepLine"></div>
      <div class="dist-value" id="distNum">---</div>
      <div class="dist-unit">CM</div>
    </div>
  </div>

  <div class="status-line">
    <div class="status-dot" id="statusDot"></div>
    <div class="status-text" id="statusText">Connecting...</div>
  </div>

  <div class="alert-box" id="alertBox">
    <span id="alertMsg">Press start to enable voice alerts</span>
  </div>

  <button class="btn" id="btn" onclick="toggleVoice()">Start Voice Alerts</button>

  <div class="legend">
    <div class="legend-item">
      <div class="legend-range" style="color:var(--danger)">0 – 20 cm</div>
    </div>
    <div class="legend-item">
      <div class="legend-range" style="color:var(--caution)">20 – 60 cm</div>
    </div>
    <div class="legend-item">
      <div class="legend-range" style="color:var(--safe)">&gt; 60 cm</div>
    </div>
  </div>

  <div class="divider"></div>
  <div class="footer">Keep screen on &middot; Voice runs in background</div>

</div>
<script>
let voiceOn = false, lastZone = '', isSpeaking = false, wakeLock = null;

function fetchDist() {
  fetch('/distance')
    .then(r => r.json())
    .then(d => update(d.distance))
    .catch(() => {
      document.getElementById('distNum').textContent = '---';
      document.getElementById('statusText').textContent = 'Reconnecting...';
    });
}
setInterval(fetchDist, 500);

function update(dist) {
  const num    = document.getElementById('distNum');
  const stTxt  = document.getElementById('statusText');
  const stDot  = document.getElementById('statusDot');
  const box    = document.getElementById('alertBox');
  const msg    = document.getElementById('alertMsg');
  const ring   = document.getElementById('radarRing');
  const sweep  = document.getElementById('sweepLine');

  let zone, color, label, alert;

  if (dist >= 999) {
    zone = 'clear'; color = 'var(--safe)';
    label = 'Path Clear'; alert = 'No obstacle detected';
    num.textContent = '---';
  } else {
    num.textContent = dist;
    if (dist <= 20) {
      zone = 'danger'; color = 'var(--danger)';
      label = 'Stop — Obstacle Very Close'; alert = 'Obstacle within 20 cm. Stop immediately.';
    } else if (dist <= 60) {
      zone = 'caution'; color = 'var(--caution)';
      label = 'Caution — Slow Down'; alert = 'Obstacle detected at ' + dist + ' cm. Proceed carefully.';
    } else {
      zone = 'clear'; color = 'var(--safe)';
      label = 'Path Clear'; alert = 'No obstacle in range.';
    }
  }

  num.style.color = color;
  stTxt.style.color = color;
  stTxt.textContent = label;
  stDot.style.background = color;
  stDot.style.boxShadow = '0 0 8px ' + color;
  stDot.className = 'status-dot' + (zone !== 'clear' ? ' pulse' : '');
  ring.style.borderColor = zone === 'clear' ? 'var(--border)' : color;
  ring.style.boxShadow   = zone === 'clear' ? 'none' : '0 0 24px ' + color + '30';
  sweep.style.background = 'conic-gradient(from 0deg, transparent 0%, ' + color + '20 12%, transparent 12%)';
  box.style.borderColor  = zone === 'clear' ? 'var(--border)' : color + '55';
  box.style.color        = zone === 'clear' ? 'var(--muted)' : color;
  msg.textContent = alert;

  if (voiceOn) speak(zone);
}

function speak(zone) {
  if (zone === 'clear') return;
  // Never cut off a sentence already playing
  if (isSpeaking) return;
  // Only repeat the same zone alert after the sentence finishes + a cooldown gap
  if (zone === lastZone) return;
  lastZone = zone;
  isSpeaking = true;
  const text = zone === 'danger'
    ? 'Danger. Obstacle very close. Stop immediately.'
    : 'Caution. Obstacle ahead. Slow down.';
  const u = new SpeechSynthesisUtterance(text);
  u.lang = 'en-US'; u.rate = 1.0; u.volume = 1.0;
  // Only allow the next alert after this one fully ends + cooldown
  u.onend = () => {
    const cooldown = 1000;
    setTimeout(() => {
      isSpeaking = false;
      // Reset zone so the same zone can fire again after cooldown
      if (lastZone === zone) lastZone = '';
    }, cooldown);
  };
  u.onerror = () => { isSpeaking = false; lastZone = ''; };
  window.speechSynthesis.speak(u);
}

function toggleVoice() {
  voiceOn = !voiceOn;
  const btn = document.getElementById('btn');
  if (voiceOn) {
    btn.textContent = 'Stop Voice Alerts';
    btn.classList.add('on');
    navigator.wakeLock && navigator.wakeLock.request('screen')
      .then(w => wakeLock = w).catch(() => {});
    const u = new SpeechSynthesisUtterance('Voice alerts activated.');
    u.lang = 'en-US'; window.speechSynthesis.speak(u);
  } else {
    btn.textContent = 'Start Voice Alerts';
    btn.classList.remove('on');
    window.speechSynthesis.cancel();
    isSpeaking = false; lastZone = '';
    if (wakeLock) { wakeLock.release(); wakeLock = null; }
  }
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", PAGE_HTML);
}

void handleDistance() {
  long dist = getDistance();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json",
    "{\"distance\":" + String(dist) + "}");
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\n Connected!");
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/distance", handleDistance);
  server.begin();

  for(int i=0;i<3;i++){
    digitalWrite(BUZZER_PIN,HIGH); delay(100);
    digitalWrite(BUZZER_PIN,LOW);  delay(100);
  }
}

void loop() {
  server.handleClient();
  long dist = getDistance();
  handleBuzzer(dist);

  static unsigned long lastPrint = 0;
  if(millis()-lastPrint>300){
    Serial.print("Distance: ");
    if(dist==999) Serial.println("Out of range");
    else { Serial.print(dist); Serial.println(" cm"); }
    lastPrint=millis();
  }
}
