(function () {
  'use strict';

  console.log("CGI SCRIPT LOADED");

  function $(id) {
    return document.getElementById(id);
  }

  function setLoading(btn, on) {
    if (!btn) return;
    btn.disabled = on;
  }

  /* ─────────────────────────────
     NAV SYSTEM
  ───────────────────────────── */
  var navItems = document.querySelectorAll('.nav-item');
  var panels = document.querySelectorAll('.panel');

  function showPanel(id) {
    panels.forEach(function (p) {
      p.classList.toggle('active', p.id === id);
    });

    navItems.forEach(function (n) {
      n.classList.toggle('active', n.dataset.panel === id);
    });
  }

  navItems.forEach(function (item) {
    item.addEventListener('click', function () {
      showPanel(item.dataset.panel);
    });
  });

  showPanel('panel-upload');

  /* ─────────────────────────────
     UPLOAD
  ───────────────────────────── */
  var uploadInput = document.querySelector('#drop-upload input[type=file]');
  var btnUpload = $('btn-upload');
  var uploadZone = document.getElementById('drop-upload');
  var uploadInput = uploadZone.querySelector('input[type=file]');
  var uploadLabel = uploadZone.querySelector('.drop-label');

  if (uploadInput && uploadLabel) {
    uploadInput.addEventListener('change', function () {
      var file = uploadInput.files[0];
  
      if (file) {
        uploadLabel.textContent = file.name;
        uploadZone.classList.add('has-file');
      }
    });
  }
  function getUploadFile() {
    return uploadInput && uploadInput.files ? uploadInput.files[0] : null;
  }

  if (btnUpload) {
    btnUpload.addEventListener('click', function () {
      console.log("btn upload clicked");
      var file = getUploadFile();
      console.log(file);
      if (!file) return alert("select file");

      setLoading(btnUpload, true);

      var fd = new FormData();
      fd.append('file', file);

      fetch('/cgi/upload', {
        method: 'POST',
        body: fd
      })
        .then(r => r.text())
        .then(t => console.log("UPLOAD:", t))
        .catch(console.error)
        .finally(() => setLoading(btnUpload, false));
    });
  }

  /* ─────────────────────────────
     DELETE (FIXED ID)
  ───────────────────────────── */
  var btnDelete = $('btn-delete');
  var deleteInput = $('delete-file');

  if (btnDelete) {
    btnDelete.addEventListener('click', function () {
      if (!deleteInput || !deleteInput.value) {
        alert("enter filename");
        return;
      }

      setLoading(btnDelete, true);

      fetch('/cgi/delete', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: 'path=' + encodeURIComponent(deleteInput.value)
      })
        .then(r => r.text())
        .then(t => console.log("DELETE:", t))
        .catch(console.error)
        .finally(() => setLoading(btnDelete, false));
    });
  }

})();
