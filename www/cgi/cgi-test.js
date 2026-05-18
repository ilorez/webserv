/* cgi-test.js */

// ─── nav routing ──────────────────────────────────────────────────────────────
const navItems = document.querySelectorAll('.nav-item');
const panels   = document.querySelectorAll('.panel');

function showPanel(id) {
  panels.forEach(p => p.classList.toggle('active', p.id === id));
  navItems.forEach(n => n.classList.toggle('active', n.dataset.panel === id));
}

navItems.forEach(item => {
  item.addEventListener('click', () => showPanel(item.dataset.panel));
});

// activate first by default
showPanel('panel-upload');

// ─── helpers ──────────────────────────────────────────────────────────────────
function setLoading(btn, on) {
  btn.disabled = on;
  btn.classList.toggle('loading', on);
}

function renderResponse(wrapId, { status, body, ms, isImage, blob }) {
  const wrap = document.getElementById(wrapId);
  const cls  = status >= 200 && status < 300 ? 's-ok' : 's-err';
  const bodyEl = wrap.querySelector('.res-body');

  wrap.querySelector('.res-meta').innerHTML =
    `<span class="status-pill ${cls}">HTTP ${status}</span>` +
    `<span class="res-time">${ms}ms</span>`;

  if (isImage && blob) {
    const url = URL.createObjectURL(blob);
    bodyEl.innerHTML = `<img src="${url}" alt="CGI response image" />`;
  } else {
    bodyEl.textContent = body || '(empty response)';
  }
}

function responseHTML() {
  return `
    <div class="response-wrap">
      <div class="res-meta"><span class="status-pill s-pending">waiting…</span></div>
      <div class="res-body"></div>
    </div>`;
}

// ─── drop zone helper ─────────────────────────────────────────────────────────
function initDropZone(zoneId) {
  const zone  = document.getElementById(zoneId);
  const input = zone.querySelector('input[type=file]');
  const label = zone.querySelector('.drop-label');

  input.addEventListener('change', () => updateLabel());
  zone.addEventListener('dragover', e => { e.preventDefault(); zone.classList.add('over'); });
  zone.addEventListener('dragleave', () => zone.classList.remove('over'));
  zone.addEventListener('drop', e => {
    e.preventDefault();
    zone.classList.remove('over');
    if (e.dataTransfer.files.length) {
      input.files = e.dataTransfer.files;
      updateLabel();
    }
  });

  function updateLabel() {
    const f = input.files[0];
    if (f) {
      label.textContent = f.name;
      zone.classList.add('has-file');
    }
  }
  return () => input.files[0];
}

// ─── 1. UPLOAD ────────────────────────────────────────────────────────────────
const getUploadFile = initDropZone('drop-upload');

document.getElementById('btn-upload').addEventListener('click', async () => {
  const file = getUploadFile();
  const dest = document.getElementById('upload-dest').value.trim();
  const btn  = document.getElementById('btn-upload');

  if (!file) { alert('select a file first'); return; }

  setLoading(btn, true);
  const fd = new FormData();
  fd.append('file', file);
  if (dest) fd.append('destination', dest);

  const t0 = Date.now();
  try {
    const r    = await fetch('/cgi/upload', { method: 'POST', body: fd });
    const body = await r.text();
    renderResponse('res-upload', { status: r.status, body, ms: Date.now() - t0 });
  } catch(e) {
    renderResponse('res-upload', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});

// ─── 2. DELETE ────────────────────────────────────────────────────────────────
document.getElementById('btn-delete').addEventListener('click', async () => {
  const path = document.getElementById('delete-path').value.trim();
  const btn  = document.getElementById('btn-delete');
  if (!path) { alert('enter a path to delete'); return; }

  setLoading(btn, true);
  const t0 = Date.now();
  try {
    const r    = await fetch('/cgi/delete', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: `path=${encodeURIComponent(path)}`
    });
    const body = await r.text();
    renderResponse('res-delete', { status: r.status, body, ms: Date.now() - t0 });
  } catch(e) {
    renderResponse('res-delete', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});

// ─── 3. RUN — image compress ──────────────────────────────────────────────────
const getCompressFile = initDropZone('drop-compress');

document.getElementById('btn-compress').addEventListener('click', async () => {
  const file    = getCompressFile();
  const quality = document.getElementById('compress-quality').value;
  const btn     = document.getElementById('btn-compress');

  if (!file) { alert('select an image first'); return; }

  setLoading(btn, true);
  const fd = new FormData();
  fd.append('file', file);
  fd.append('quality', quality);
  fd.append('action', 'compress_image');

  const t0 = Date.now();
  try {
    const r = await fetch('/cgi/run', { method: 'POST', body: fd });
    const contentType = r.headers.get('Content-Type') || '';
    const ms = Date.now() - t0;
    if (contentType.startsWith('image/')) {
      const blob = await r.blob();
      renderResponse('res-compress', { status: r.status, ms, isImage: true, blob });
    } else {
      const body = await r.text();
      renderResponse('res-compress', { status: r.status, body, ms });
    }
  } catch(e) {
    renderResponse('res-compress', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});

// quality slider readout
const qualitySlider = document.getElementById('compress-quality');
const qualityOut    = document.getElementById('compress-quality-out');
if (qualitySlider) {
  qualitySlider.addEventListener('input', () => {
    qualityOut.textContent = qualitySlider.value + '%';
  });
}

// ─── 4. RUN — word filter ─────────────────────────────────────────────────────

// tags input
const tagInput  = document.getElementById('tag-input');
const tagsWrap  = document.getElementById('tags-wrap');
let   filterWords = [];

function addTag(word) {
  word = word.trim().replace(/,/g, '');
  if (!word || filterWords.includes(word)) return;
  filterWords.push(word);
  const pill = document.createElement('span');
  pill.className = 'tag-pill';
  pill.innerHTML = `${escHtml(word)}<button type="button" aria-label="remove ${escHtml(word)}">×</button>`;
  pill.querySelector('button').addEventListener('click', () => {
    filterWords = filterWords.filter(w => w !== word);
    pill.remove();
  });
  tagsWrap.insertBefore(pill, tagInput);
}

function escHtml(s) {
  return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

tagInput.addEventListener('keydown', e => {
  if (['Enter', ',', ' '].includes(e.key)) {
    e.preventDefault();
    addTag(tagInput.value);
    tagInput.value = '';
  }
  if (e.key === 'Backspace' && !tagInput.value && filterWords.length) {
    const last = filterWords[filterWords.length - 1];
    filterWords.pop();
    tagsWrap.querySelectorAll('.tag-pill').forEach(p => {
      if (p.textContent.replace('×','').trim() === last) p.remove();
    });
  }
});
tagsWrap.addEventListener('click', () => tagInput.focus());

document.getElementById('btn-filter').addEventListener('click', async () => {
  const text  = document.getElementById('filter-text').value.trim();
  const words = filterWords.slice();
  const btn   = document.getElementById('btn-filter');

  if (!text) { alert('enter some text first'); return; }
  if (!words.length) { alert('add at least one word to filter'); return; }

  setLoading(btn, true);
  const fd = new FormData();
  fd.append('text', text);
  fd.append('words', words.join(','));
  fd.append('action', 'filter_words');

  const t0 = Date.now();
  try {
    const r    = await fetch('/cgi/run', { method: 'POST', body: fd });
    const body = await r.text();
    renderResponse('res-filter', { status: r.status, body, ms: Date.now() - t0 });
  } catch(e) {
    renderResponse('res-filter', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});

// ─── 5. RUN — GET custom ─────────────────────────────────────────────────────
document.getElementById('btn-run-get').addEventListener('click', async () => {
  const qs  = document.getElementById('run-qs').value.trim();
  const btn = document.getElementById('btn-run-get');
  const url = '/cgi/run' + (qs ? '?' + qs : '');

  setLoading(btn, true);
  const t0 = Date.now();
  try {
    const r    = await fetch(url, { method: 'GET' });
    const body = await r.text();
    renderResponse('res-run', { status: r.status, body, ms: Date.now() - t0 });
  } catch(e) {
    renderResponse('res-run', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});

// ─── 6. RUN — POST custom ────────────────────────────────────────────────────
const getCustomFile = initDropZone('drop-custom');

document.getElementById('btn-run-post').addEventListener('click', async () => {
  const qs    = document.getElementById('run-post-qs').value.trim();
  const extra = document.getElementById('run-post-body').value.trim();
  const file  = getCustomFile();
  const btn   = document.getElementById('btn-run-post');
  const url   = '/cgi/run' + (qs ? '?' + qs : '');

  setLoading(btn, true);
  let body;
  if (file) {
    const fd = new FormData();
    fd.append('file', file);
    if (extra) {
      extra.split('\n').forEach(line => {
        const [k, ...v] = line.split('=');
        if (k && v.length) fd.append(k.trim(), v.join('=').trim());
      });
    }
    body = fd;
  } else if (extra) {
    body = extra;
  }

  const t0 = Date.now();
  try {
    const r    = await fetch(url, { method: 'POST', body });
    const body2 = await r.text();
    renderResponse('res-run', { status: r.status, body: body2, ms: Date.now() - t0 });
  } catch(e) {
    renderResponse('res-run', { status: 0, body: `Error: ${e.message}`, ms: Date.now() - t0 });
  } finally { setLoading(btn, false); }
});
