#include "WebPage.h"

const char INDEX_HTML[] PROGMEM = R"FILMHTML(
<!doctype html>
<html lang="zh-Hant">

<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title>Film Memory</title>
    <style>
        :root {
            --bg-color: #f7f3eb;
            --card-bg: #f0ebe1;
            --text-main: #2b2621;
            --text-muted: #786f66;
            --accent-gold: #b8860b;
            --accent-border: #dcd3c5;
            --btn-pick: #2b2621;
            --btn-pick-text: #f7f3eb;
            --btn-sub: #e2dad0;
            --btn-danger: #8b3a3a;
        }

        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            background-color: var(--bg-color);
            color: var(--text-main);
            font-family: "Noto Serif TC", "Songti TC", "Georgia", serif;
            -webkit-font-smoothing: antialiased;
        }

        main {
            max-width: 620px;
            margin: 0 auto;
            padding: 40px 20px 80px;
        }

        .brand {
            font-family: system-ui, -apple-system, sans-serif;
            font-size: 0.75rem;
            letter-spacing: 0.25em;
            color: var(--accent-gold);
            font-weight: 700;
            text-transform: uppercase;
            margin-bottom: 6px;
        }

        h1 {
            font-size: 1.85rem;
            font-weight: 600;
            margin: 0 0 10px 0;
            letter-spacing: 0.05em;
        }

        p {
            line-height: 1.7;
            color: var(--text-muted);
            font-size: 0.95rem;
            margin: 0 0 20px 0;
        }

        .card {
            border: 1px solid var(--accent-border);
            border-radius: 12px;
            padding: 24px;
            margin-top: 20px;
            background: var(--card-bg);
            box-shadow: 0 4px 20px rgba(43, 38, 33, 0.03);
            /* 輕微柔和陰影 */
        }

        button,
        .pick {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            border: 0;
            border-radius: 6px;
            padding: 10px 20px;
            font-family: inherit;
            font-size: 0.9rem;
            font-weight: 600;
            letter-spacing: 0.03em;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .pick {
            background: var(--btn-pick);
            color: var(--btn-pick-text);
        }

        .pick:hover {
            opacity: 0.9;
        }

        button {
            background: var(--btn-sub);
            color: var(--text-main);
        }

        button:hover:not(:disabled) {
            background: #d5ccc0;
        }

        button:disabled {
            opacity: 0.4;
            cursor: not-allowed;
        }

        .danger {
            background: var(--btn-danger);
            color: #f7f3eb;
        }

        .danger:hover {
            opacity: 0.9;
        }

        input {
            display: none;
        }

        #preview {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(80px, 1fr));
            gap: 12px;
            margin-top: 20px;
        }

        /* 拍立得照片邊框效果 */
        #preview img {
            width: 100%;
            aspect-ratio: 1;
            object-fit: cover;
            border-radius: 4px;
            background: #e0d8cc;
            padding: 4px;
            border: 1px solid #d0c5b5;
            box-shadow: 0 2px 6px rgba(0, 0, 0, 0.06);
        }

        .bar {
            height: 4px;
            border-radius: 2px;
            background: var(--accent-border);
            overflow: hidden;
            margin-top: 18px;
        }

        .bar i {
            display: block;
            height: 100%;
            width: 0;
            background: var(--accent-gold);
            transition: width 0.3s ease;
        }

        .row {
            display: flex;
            gap: 12px;
            flex-wrap: wrap;
            align-items: center;
        }

        .small {
            font-size: 0.85rem;
            color: var(--text-muted);
        }

        .photo-list {
            display: grid;
            gap: 8px;
            margin-top: 16px;
        }

        .photo-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 12px;
            padding: 9px 10px;
            border: 1px solid var(--accent-border);
            border-radius: 6px;
            background: var(--bg-color);
        }

        .photo-info {
            display: flex;
            align-items: center;
            min-width: 0;
            gap: 10px;
        }

        .stored-thumb {
            width: 58px;
            height: 58px;
            flex: 0 0 58px;
            object-fit: cover;
            border-radius: 4px;
            border: 1px solid var(--accent-border);
            background: #e0d8cc;
        }

        .photo-name {
            font-family: monospace;
            font-size: 0.82rem;
            overflow-wrap: anywhere;
        }

        .delete-one {
            padding: 7px 12px;
            background: var(--btn-danger);
            color: #f7f3eb;
        }

        code {
            font-family: monospace;
            color: var(--accent-gold);
            background: rgba(184, 134, 11, 0.08);
            padding: 2px 6px;
            border-radius: 4px;
        }
    </style>
</head>

<body>
    <main>
        <div class="brand">FILM MEMORY</div>
        <h1>相片管理器</h1>
        <p>選擇照片後，瀏覽器將自動裁切為正方形並予以壓縮，保存至記憶相框中。</p>
        <section class="card">
            <div class="row">
                <label class="pick" for="files">選擇照片</label>
                <input id="files" type="file" accept="image/jpeg,image/png,image/webp" multiple>
                <button id="upload" disabled>裁切並上傳</button>
            </div>
            <div id="preview"></div>
            <div class="bar"><i id="progress"></i></div>
            <p id="status" class="small" style="margin-top: 12px;">等待選擇照片...</p>
        </section>
        <section class="card">
            <div class="row">
                <button id="refresh">更新列表</button>
                <button id="clear" class="danger">清空相冊</button>
            </div>
            <p id="stored" class="small" style="margin-top: 12px;">讀取中...</p>
            <div id="album" class="photo-list"></div>
        </section>
        <p class="small" style="margin-top: 28px; text-align: center;">提示：日常使用長按旋鈕可開啟此設定頁面 (IP:
            <code>192.168.4.1</code>)</p>
    </main>
    <script>
        const input = document.querySelector('#files'), preview = document.querySelector('#preview'), upload = document.querySelector('#upload');
        const statusEl = document.querySelector('#status'), progress = document.querySelector('#progress'), stored = document.querySelector('#stored');
        const album = document.querySelector('#album');
        let selected = [], storedCount = 0, albumMax = 50;

        // 使用 Canvas 在瀏覽器端將圖片裁切為正方形 JPEG
        function canvasBlob(file) {
            return new Promise((resolve, reject) => {
                const img = new Image(), url = URL.createObjectURL(file);
                img.onload = () => {
                    const s = Math.min(img.naturalWidth, img.naturalHeight),
                        sx = (img.naturalWidth - s) / 2,
                        sy = (img.naturalHeight - s) / 2,
                        c = document.createElement('canvas');
                    c.width = c.height = 128;
                    const x = c.getContext('2d');
                    x.fillStyle = '#000';
                    x.fillRect(0, 0, 128, 128);
                    x.drawImage(img, sx, sy, s, s, 0, 0, 128, 128);
                    URL.revokeObjectURL(url);
                    c.toBlob(b => b ? resolve(b) : reject(Error('圖片處理失敗')), 'image/jpeg', .82)
                };
                img.onerror = reject;
                img.src = url;
            });
        }

        // 選擇照片時觸發預覽
        input.onchange = () => {
            selected = [...input.files];
            preview.innerHTML = '';
            selected.forEach(f => {
                const i = document.createElement('img');
                i.src = URL.createObjectURL(f);
                preview.append(i);
            });
            upload.disabled = !selected.length;
            statusEl.textContent = `已選擇 ${selected.length} 張照片`;
        };

        // 使用瀏覽器的本機日期時間產生排序穩定、且不會重複使用空號的檔名。
        function photoFileName(timeMs) {
            const d = new Date(timeMs), pad = (value, length = 2) => String(value).padStart(length, '0');
            return `${d.getFullYear()}${pad(d.getMonth() + 1)}${pad(d.getDate())}_${pad(d.getHours())}${pad(d.getMinutes())}${pad(d.getSeconds())}_${pad(d.getMilliseconds(), 3)}.jpg`;
        }

        // 點擊上傳按鈕：依序處理並上傳照片
        upload.onclick = async () => {
            upload.disabled = true;
            try {
                await refresh(false);
                if (storedCount + selected.length > albumMax) {
                    throw Error(`最多可存 ${albumMax} 張，目前已有 ${storedCount} 張`);
                }
                const batchStartedAt = Date.now();
                for (let n = 0; n < selected.length; n++) {
                    statusEl.textContent = `處理並上傳中 ${n + 1} / ${selected.length}`;
                    const blob = await canvasBlob(selected[n]),
                        name = photoFileName(batchStartedAt + n),
                        form = new FormData();
                    form.append('file', blob, name);
                    const r = await fetch('/upload', { method: 'POST', body: form });
                    if (!r.ok) throw Error(await r.text());
                    progress.style.width = ((n + 1) / selected.length * 100) + '%';
                }
                statusEl.textContent = '上傳完成！';
                selected = [];
                input.value = '';
                preview.innerHTML = '';
                await refresh(false);
            } catch (e) {
                statusEl.textContent = '失敗：' + e.message;
            } finally {
                upload.disabled = false;
            }
        };

        // 刷新並取得 ESP32 儲存空間狀況
        async function refresh(showStatus = false) {
            try {
                const r = await fetch('/list?t=' + Date.now(), { cache: 'no-store' });
                if (!r.ok) throw Error('讀取失敗');
                const j = await r.json();
                storedCount = j.count;
                albumMax = j.max || 50;
                stored.textContent = `已儲存 ${j.count} / ${albumMax} 張照片，使用 ${Math.round(j.used / 1024)} KB / ${Math.round(j.total / 1024)} KB`;
                album.innerHTML = '';
                if (!j.files || !j.files.length) {
                    const empty = document.createElement('span');
                    empty.className = 'small';
                    empty.textContent = '相冊目前沒有照片';
                    album.append(empty);
                } else {
                    j.files.forEach(name => {
                        const row = document.createElement('div');
                        row.className = 'photo-item';
                        const info = document.createElement('div');
                        info.className = 'photo-info';
                        const thumb = document.createElement('img');
                        thumb.className = 'stored-thumb';
                        thumb.loading = 'lazy';
                        thumb.alt = name;
                        thumb.src = '/photo?name=' + encodeURIComponent(name) + '&t=' + Date.now();
                        const label = document.createElement('span');
                        label.className = 'photo-name';
                        label.textContent = name;
                        const button = document.createElement('button');
                        button.className = 'delete-one';
                        button.textContent = '刪除';
                        button.onclick = () => deletePhoto(name, button);
                        info.append(thumb, label);
                        row.append(info, button);
                        album.append(row);
                    });
                }
                if (showStatus) statusEl.textContent = '列表已更新';
            } catch (e) {
                stored.textContent = '無法讀取照片列表：' + e.message;
            }
        }

        async function deletePhoto(name, button) {
            if (!confirm(`確定要刪除 ${name} 嗎？`)) return;
            button.disabled = true;
            try {
                const r = await fetch('/delete?name=' + encodeURIComponent(name), { method: 'POST' });
                if (!r.ok) throw Error(await r.text());
                statusEl.textContent = `已刪除 ${name}`;
                await refresh(false);
            } catch (e) {
                statusEl.textContent = '刪除失敗：' + e.message;
                button.disabled = false;
            }
        }

        document.querySelector('#refresh').onclick = () => refresh(true);
        document.querySelector('#clear').onclick = async () => {
            if (confirm('確定要清空所有照片嗎？')) {
                await fetch('/clear', { method: 'POST' });
                progress.style.width = '0';
                await refresh(true);
            }
        };
        async function syncClock() {
            try {
                await fetch('/time?epoch=' + Math.floor(Date.now() / 1000), { method: 'POST' });
            } catch (_) {
                // 相片管理仍可使用；下次開啟網頁會再次同步。
            }
        }

        syncClock();
        refresh(false);
    </script>
</body>

</html>
)FILMHTML";
