(function () {
  'use strict';

  /* ── elements ── */
  var dropZone    = document.getElementById('dropZone');
  var fileInput   = document.getElementById('fileInput');
  var previewWrap = document.getElementById('previewWrap');
  var previewImg  = document.getElementById('previewImg');
  var fileNameEl  = document.getElementById('fileName');
  var uploadBtn   = document.getElementById('uploadBtn');
  var getBtn      = document.getElementById('getBtn');
  var deleteBtn   = document.getElementById('deleteBtn');
  var getNameEl   = document.getElementById('getImageName');
  var delNameEl   = document.getElementById('deleteImageName');

  /* ── helpers ── */
  function showMsg(containerId, text, type) {
    document.getElementById(containerId).innerHTML =
      '<div class="msg msg-' + type + '">' + text + '</div>';
  }

  function setPreview(file) {
    previewImg.src      = URL.createObjectURL(file);
    fileNameEl.textContent = file.name;
    previewWrap.style.display = 'block';
    document.getElementById('uploadResult').innerHTML = '';
  }

  /* ── drag & drop ── */
  dropZone.addEventListener('dragover', function (e) {
    e.preventDefault();
    dropZone.classList.add('dragover');
  });

  dropZone.addEventListener('dragleave', function () {
    dropZone.classList.remove('dragover');
  });

  dropZone.addEventListener('drop', function (e) {
    e.preventDefault();
    dropZone.classList.remove('dragover');
    var f = e.dataTransfer.files[0];
    if (f) setPreview(f);
  });

  fileInput.addEventListener('change', function () {
    if (fileInput.files[0]) setPreview(fileInput.files[0]);
  });

  /* ── upload ── */
  uploadBtn.addEventListener('click', function () {
    var file = fileInput.files[0];
    if (!file) { showMsg('uploadResult', 'no file selected', 'err'); return; }

    var allowed = ['image/jpeg', 'image/png'];
    if (allowed.indexOf(file.type) === -1) {
      showMsg('uploadResult', 'only jpg / jpeg / png allowed', 'err');
      return;
    }

    showMsg('uploadResult', 'uploading…', 'info');

    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/upload/' + encodeURIComponent(file.name));
    xhr.setRequestHeader('Content-Type', file.type);

    xhr.onload = function () {
      if (xhr.status >= 200 && xhr.status < 300)
        showMsg('uploadResult', 'uploaded → ' + file.name, 'ok');
      else
        showMsg('uploadResult', 'server error ' + xhr.status, 'err');
    };

    xhr.onerror = function () {
      showMsg('uploadResult', 'request failed', 'err');
    };

    xhr.send(file);
  });

  /* ── get image ── */
  getBtn.addEventListener('click', function () {
    var name = getNameEl.value.trim();
    if (!name) { showMsg('getResult', 'enter a name', 'err'); return; }

    showMsg('getResult', 'fetching…', 'info');

    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/upload/' + encodeURIComponent(name));
    xhr.responseType = 'blob';

    xhr.onload = function () {
      if (xhr.status >= 200 && xhr.status < 300) {
        var url = URL.createObjectURL(xhr.response);
        document.getElementById('getResult').innerHTML =
          '<img src="' + url + '" alt="' + name + '">';
      } else {
        showMsg('getResult', 'not found (' + xhr.status + ')', 'err');
      }
    };

    xhr.onerror = function () {
      showMsg('getResult', 'request failed', 'err');
    };

    xhr.send();
  });

  /* ── delete ── */
  deleteBtn.addEventListener('click', function () {
    var name = delNameEl.value.trim();
    if (!name) { showMsg('deleteResult', 'enter a name', 'err'); return; }

    showMsg('deleteResult', 'deleting…', 'info');

    var xhr = new XMLHttpRequest();
    xhr.open('DELETE', '/upload/' + encodeURIComponent(name));

    xhr.onload = function () {
      if (xhr.status >= 200 && xhr.status < 300)
        showMsg('deleteResult', 'deleted → ' + name, 'ok');
      else
        showMsg('deleteResult', 'error ' + xhr.status, 'err');
    };

    xhr.onerror = function () {
      showMsg('deleteResult', 'request failed', 'err');
    };

    xhr.send();
  });

})();
